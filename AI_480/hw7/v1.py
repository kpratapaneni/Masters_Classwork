class StudentAgent(Agent):
    """The Student Agent"""
    def __init__(self, name):
        super(StudentAgent, self).__init__(name)
        self.prob_being_junk = -99
        self.count = 0
        self.expected_profit = 0
    
    def will_buy(self, value, price, prob):
        # value - selling price
        # price - cost price
        #  profit = sp - cp (i.e. value-price)
        # CHANGE THIS. IMPLEMENT THE BODY OF THIS METHOD

        # print(value, price, prob)

        if(self.prob_being_junk == -99):
            self.prob_being_junk = prob
            self.count += 1
            return True
        else:
            avg = ((self.prob_being_junk * self.count) + prob) / (self.count+1)
            self.prob_being_junk = avg
            self.count += 1
            if(prob < self.prob_being_junk):
                return True
            else:
                return False

        print("****************** I can never reach this point ******************")
        if(prob < self.prob_being_junk):
            return True
        else:
            self.prob_being_junk = prob;
            return False
        return False

# V2
class StudentAgent(Agent):
    """The Student Agent"""
    def __init__(self, name):
        super(StudentAgent, self).__init__(name)
        self.prob_being_junk = -99
        self.count = 0
        self.expected_profit = -99
    
    def will_buy(self, value, price, prob):
        # value - selling price
        # price - cost price
        #  profit = sp - cp (i.e. value-price)
        # CHANGE THIS. IMPLEMENT THE BODY OF THIS METHOD

        # print(value, price, prob)

        if(self.prob_being_junk == -99):
            self.prob_being_junk = prob
            self.count += 1
            self.expected_profit = (value - price)
            return True
        else:
            avg = ((self.prob_being_junk * self.count) + prob) / (self.count+1)
            self.prob_being_junk = avg
            self.count += 1
            
            self.expected_profit = ((self.expected_profit * self.count) + (value - price)) / (self.count+1)

            if((value - price) > self.expected_profit):
                return True
            else:
                return False

        print("****************** I can never reach this point ******************")
        if(prob < self.prob_being_junk):
            return True
        else:
            self.prob_being_junk = prob;
            return False
        return False

class StudentAgent(Agent):
    """The Student Agent"""
    def __init__(self, name):
        super(StudentAgent, self).__init__(name)
        self.prob_being_junk = -99
        self.count = 0
        self.expected_profit = -99
    
    def will_buy(self, value, price, prob):
        # value - selling price
        # price - cost price
        #  profit = sp - cp (i.e. value-price)
        # CHANGE THIS. IMPLEMENT THE BODY OF THIS METHOD

        # print(value, price, prob)

        # if(prob > 0.5):
        #     return False
        # price/value

        test = (price/value)*prob

        if(self.prob_being_junk == -99):
            self.prob_being_junk = prob
            self.count += 1
            self.expected_profit = test
            return True
        else:
            avg = ((self.prob_being_junk * self.count) + prob) / (self.count+1)
            self.prob_being_junk = avg
            self.count += 1
            
            self.expected_profit = ((self.expected_profit * self.count) + (test)) / (self.count+1)

            if((test) < self.expected_profit):
                return True
            else:
                return False

        print("****************** I can never reach this point ******************")
        if(prob < self.prob_being_junk):
            return True
        else:
            self.prob_being_junk = prob;
            return False
        return False