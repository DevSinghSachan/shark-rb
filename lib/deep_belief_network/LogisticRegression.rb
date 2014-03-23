#  Logistic Regression
 
#  References :
#    -Yusuke Sugomori
#    https://github.com/yusugomori/DeepLearning
#
#    - Jason Rennie: Logistic Regression,
#    http://qwone.com/~jason/writing/lr.pdf
 
#    - DeepLearningTutorials
#    https://github.com/lisa-lab/DeepLearningTutorials

require 'shark-rb'

class Optimizer
    class LogisticRegression

        attr_accessor :parameters
        attr_accessor :bias
        attr_reader   :number_of_inputs
        attr_reader   :number_of_outputs

        def initialize opts={}
            @input             = opts[:samples]
            @labels            = opts[:labels]
            @number_of_inputs  = opts[:number_of_inputs]
            @number_of_outputs = opts[:number_of_outputs]
            @parameters        = Shark::RealMatrix.new @number_of_inputs, @number_of_outputs
            @bias              = Shark::RealVector.new @number_of_outputs
        end

        # deprecated Softmax, causes numerical errors... why? who knows. TODO: find out why. (March 5th 2014)
        # def softmax x
        #     if x.is_a?(Array) then return softmax(x.to_realvector) end
        #     e = (x - x.max).exp
        #     case e
        #     when Shark::RealVector, Shark::RealVectorReference, Shark::RealMatrixColumn, Shark::RealMatrixRow
        #         # if e.to_a.any? {|i| i.nan?} then raise StandardError.new "Has NaN, array case #{e.to_a}" end
        #         e / e.sum  # sum columns...
        #     else
        #         # if e.to_a.flatten.any? {|i| i.nan?} then raise StandardError.new "e contains NaN, #{e.to_a}" end
        #         # if e.sum(axis:1).to_a.any? {|i| i.nan?} then raise StandardError.new "e sum contains NaN, #{e.to_a}" end
        #         e / e.sum(axis:1)
        #     end
        # end

        def output x
            if x.is_a?(Array)
                output ~(x.to_realvector.to_matrix)
            else
                soft = Shark::Softmax.new
                soft.set_structure(@number_of_outputs)
                soft.eval(x * @parameters + @bias) # this could be a matrix too!
            end
        end

        def train opts={}
            if opts[:input] then @input = opts[:input] end
            l2_reg        = opts[:l2_reg] || 0.0
            lr            = opts[:learning_rate] || 0.01

            p_y_given_x  = output @input
            d_y          = @labels - p_y_given_x
            @parameters += (lr * (~@input * d_y) - (lr * l2_reg * @parameters))
            @bias       += (lr * d_y.mean(axis:0))
        end

        def negative_log_likelihood
            activation = output @input
            # cross entropy
            (
                -(
                    @labels.hadamard(activation.log) +
                    (-@labels+1.0).hadamard((-activation + 1.0).log)
                ).sum(axis:1)
            ).mean
        end

        alias :predict :output
        alias :output_size :number_of_outputs
        alias :input_size  :number_of_inputs

    end
end

def test_lr(learning_rate=0.01, n_epochs=10)
    # training data
    x = Shark::RealMatrix.new [
        [1,1,1,0,0,0],
        [1,0,1,0,0,0],
        [1,1,1,0,0,0],
        [0,0,1,1,1,0],
        [0,0,1,1,0,0],
        [0,0,1,1,1,0]
    ]
    y = Shark::RealMatrix.new [
        [1, 0],
        [1, 0],
        [1, 0],
        [0, 1],
        [0, 1],
        [0, 1]
    ]

    # construct LogisticRegression
    classifier = Optimizer::LogisticRegression.new samples: x,
                                                   labels: y,
                                                   number_of_inputs: 6,
                                                   number_of_outputs: 2

    # train
    n_epochs.times do |epoch|
        classifier.train learning_rate: learning_rate
        learning_rate *= 0.95
        # cost = classifier.negative_log_likelihood
        # print 'Training epoch %d, cost is %f' % [epoch, cost], "\n"
    end

    # test
    x = Shark::RealMatrix.new [
        [1, 1, 0, 0, 0, 0],
        [0, 0, 0, 1, 1, 0],
        [1, 1, 1, 1, 1, 0]
    ]

    print "classifier.predict(x).to_a => \n", classifier.predict(x).to_s, "\n"
    puts classifier.predict([1, 1, 0, 0, 0, 0].to_realvector)
end

if __FILE__ == $0
    test_lr
end
