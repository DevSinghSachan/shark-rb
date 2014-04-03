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

        attr_accessor :input

        def initialize opts={}
            @input             = opts[:samples]
            @labels            = opts[:labels]
            @number_of_inputs  = opts[:number_of_inputs]
            @number_of_outputs = opts[:number_of_outputs]
            @parameters        = Shark::RealMatrix.new @number_of_inputs, @number_of_outputs
            @bias              = Shark::RealVector.new @number_of_outputs
        end

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
            @parameters += lr * ((~@input) * d_y) - lr * l2_reg * @parameters
            @bias       += lr * d_y.mean(axis:0)
            if opts[:verbose] then puts "Cost is #{negative_log_likelihood}" end
        end

        def negative_log_likelihood
            activation = output @input
            # cross entropy
            -(
                (
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
        classifier.train learning_rate: learning_rate, verbose: false
        learning_rate *= 0.95
    end

    # test
    x = Shark::RealMatrix.new [
        [1, 1, 0, 0, 0, 0],
        [0, 0, 0, 1, 1, 0],
        [1, 1, 1, 1, 1, 0]
    ]

    print "classifier.predict(x) => \n", classifier.predict(x).to_s, "\n"
end

if __FILE__ == $0
    test_lr
end
