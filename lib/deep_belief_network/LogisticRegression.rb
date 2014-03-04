# coding: utf-8

#  Logistic Regression
 
#  References :
#    - Jason Rennie: Logistic Regression,
#    http://qwone.com/~jason/writing/lr.pdf
 
#    - DeepLearningTutorials
#    https://github.com/lisa-lab/DeepLearningTutorials

require 'shark-rb'

class Optimizer
    class LogisticRegression

        def initialize opts={}
            @input             = opts[:samples]
            @labels            = opts[:labels]
            @number_of_inputs  = opts[:number_of_inputs]
            @number_of_outputs = opts[:number_of_outputs]
            @parameters        = Shark::RealMatrix.new @number_of_inputs, @number_of_outputs
            @bias              = Shark::RealVector.new @number_of_outputs
        end

        def softmax x
            e = (x - x.max).exp
            case e
            when Shark::RealVector, Shark::RealVectorReference, Shark::RealMatrixColumn, Shark::RealMatrixRow
                e / e.sum  # sum columns...
            else
                puts e.size.inspect
                puts e.sum(axis:1).size.inspect
                e / e.sum(axis:1)
            end
        end

        def output x
            if x.is_a?(Array)
                output (~x.to_realvector.to_matrix)
            else
                softmax(x * @parameters + @bias)
                # why doesnt this work??
                # soft = Shark::Softmax.new
                # soft.set_structure(@number_of_outputs)
                # soft.eval(x * @parameters + @bias) # this could be a matrix too!
            end
        end

        def train opts={}
            if opts[:input] then @input = opts[:input] end
            l2_reg        = opts[:l2_reg] || 0.0
            lr            = opts[:learning_rate] || 0.01

            p_y_given_x  = output @input
            d_y          = @labels - p_y_given_x
            puts (lr * (~@input * d_y) - lr * l2_reg * @parameters).to_a
            @parameters += lr * (~@input * d_y) - lr * l2_reg * @parameters
            @bias       += lr * d_y.mean
            # print "@bias.to_a => ", @bias.to_a, "\n"
            # print "@parameters.to_a => ", @parameters.to_a, "\n"
        end

        def negative_log_likelihood
            activation = output @input
            -(
                @labels * activation.log +
                (-@labels+1.0) * (-activation + 1.0).log
            ).sum(:columns)
        end

        alias :predict :output

    end
end


def test_lr(learning_rate=0.01, n_epochs=2)
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
    n_epochs.times do |i|
        classifier.train learning_rate: learning_rate
        learning_rate *= 0.95
    end

    # test
    x = Shark::RealMatrix.new [
        [1, 1, 0, 0, 0, 0],
        [0, 0, 0, 1, 1, 0],
        [1, 1, 1, 1, 1, 0]
    ]

    puts classifier.predict(x)
    puts classifier.output([1, 1, 0, 0, 0, 0]).to_a

end

if __FILE__ == $0
    test_lr
end
