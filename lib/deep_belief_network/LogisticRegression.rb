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
            if e.size == 1
                e / e.sum(:columns)  # sum columns...
            else
                e / ~(e.sum(:rows))
            end
        end

        def output x
             #softmax(x * @parameters + @bias)
            soft = Shark::Softmax.new
            soft.set_structure(@number_of_outputs)
            soft.eval(x * @parameters + @bias) # this could be a matrix too!
        end

        def train opts={}
            if opts[:input] then @input = opts[:input] end
            l2_reg        = opts[:l2_reg] || 0.0
            learning_rate = opts[:learning_rate] || 0.01

            p_y_given_x  = output @input
            d_y          = @labels - p_y_given_x
            @parameters += (~@parameters) * dy * learning_rate -  @parameters * learning_rate * l2_reg
            @bias       += (d_y.mean * learning_rate)
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


def test_lr(learning_rate=0.01, n_epochs=200)
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

    print classifier.predict(x)

end

if __FILE__ == $0
    test_lr
end
