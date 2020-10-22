from abc import abstractmethod

class Agent(object):
    def __init__(self, name):
        self.name = name
       
    def __str__(self):
        return "Agent_" + self.name

    @abstractmethod
    def will_buy(self, value, price, prob):
        """Given a value, price, and prob of Junk,
        return True if you want to buy it; False otherwise.
        Override this method."""

class HalfProbAgent(Agent):
    """Buys if the prob < 0.5 no matter what the value or price is"""
    
    def will_buy(self, value, price, prob):
        return (prob < 0.5)

class RatioAgent(Agent):
    """Buys if the ratio of the price to value is below a specified threshold"""
    
    def __init__(self, name, max_p_v_ratio):
        super(RatioAgent, self).__init__(name)
        self.max_p_v_ratio = max_p_v_ratio
    
    def will_buy(self, value, price, prob):
        return (price/value <= self.max_p_v_ratio)

class BuyAllAgent(Agent):
    """Simply buys all products"""
    
    def will_buy(self, value, price, prob):
        return True


class StudentAgent(Agent):
    """The Student Agent"""
    def __init__(self, name):
        super(StudentAgent, self).__init__(name)
        self.count = 0
        self.expected_avg_profit = None

    def meu(self, value, price, prob, is_junk, shall_buy):

        # junk false and buying
        if is_junk == False and shall_buy == True:
            return value
        # junk true and buy
        elif is_junk == True and shall_buy == True:
            return -(value+price)
        # junk true and buy
        elif is_junk == True and shall_buy == False:
            return price
        elif is_junk == False and shall_buy == False:
            return -value
    
    def will_buy(self, value, price, prob):
        # value - selling price
        # price - cost price
        #  profit = sp - cp (i.e. value-price)
        # CHANGE THIS. IMPLEMENT THE BODY OF THIS METHOD
        not_junk_prob = 1 - prob
        buying_prob = (prob * self.meu(value, price, prob, True, True)) + (not_junk_prob * self.meu(value, price, prob, False, True))
        not_buying_prob = (prob * self.meu(value, price, prob, True, False)) + (not_junk_prob * self.meu(value, price, prob, False, False))

        if(buying_prob > not_buying_prob):
            return True
        else:
            return False