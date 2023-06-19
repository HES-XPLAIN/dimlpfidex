import math

class StairObj:
    def __init__(self, nb_bins, hiknot):
        self.nb_bins = nb_bins
        self.nb_knots = nb_bins + 1 # Stair is <= activation

        self.hiknot = hiknot
        self.lowknot = 0.0
        self.val_lowknot = 0.0
        self.val_highknot = 0.0
        self.dist = 0.0

        self.binwidth = 0.0

        self.knots = []
        self.evalknots = []

        self.init_member_const_for_ansi()
        self.activate_knots()

    def init_member_const_for_ansi(self):
        self.lowknot = -self.hiknot
        self.dist = self.hiknot - self.lowknot

    def activate_knots(self):
        self.binwidth = self.dist / float(self.nb_bins)

        self.knots = [0.0] * self.nb_knots
        self.evalknots = [0.0] * self.nb_knots

        for k in range(self.nb_knots):
            self.knots[k] = self.lowknot + (self.binwidth * float(k))
            self.evalknots[k] = self.activation(self.knots[k])

        self.val_lowknot = self.activation(-1111111111.0)
        self.val_highknot = self.activation(self.hiknot)

    def activation(self, x):
        return self.sigmoid(x)

    def funct(self, x):
        if x < self.lowknot:
            return self.val_lowknot
        if x >= self.hiknot:
            return self.val_highknot
        indbin = int((x - self.lowknot) / self.binwidth)
        return self.evalknots[indbin]

    def sigmoid(self, x):
        try:
            return 1.0 / (1.0 + math.exp(-x))
        except OverflowError:
            if x > 0:
                return 1.0
            else:
                return 0.0
