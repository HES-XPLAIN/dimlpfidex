import math

class StairObj:
    """
    This class represents an object for manipulating stair-like structures in data.

    A `StairObj` is initialized with a number of bins (steps) and a high knot value,
    defining a range of activation where the value gradually increases across bins
    until it reaches the high knot point. The low knot point is automatically set
    to the opposite of the high knot value, creating a symmetric range of activation
    around zero. It calculates and stores knot values and their corresponding activation levels.

    Attributes:
        nb_bins (int): Number of bins (steps) in the stair object.
        nb_knots (int): Total number of knots, including both ends of the stair.
        hiknot (float): The high knot value beyond which the activation remains constant.
        lowknot (float): The low knot value from which the activation starts increasing.
        val_lowknot (float): Activation value at the low knot.
        val_highknot (float): Activation value at the high knot.
        dist (float): Distance between the high and low knots.
        binwidth (float): Width of each bin in the stair structure.
        knots (list[float]): List of knot values.
        evalknots (list[float]): List of activation values for each knot.

    Methods:
        init_member_const_for_ansi(): Initializes constants of the stair structure.
        activate_knots(): Calculates knot positions and their activation levels.
        activation(x): Computes the activation level for a given value `x`.
        funct(x): Returns the activation level for a given value `x`, considering the stair structure.
        sigmoid(x): The sigmoid activation function.
    """

    def __init__(self, nb_bins, hiknot):
        """
        Initializes a StairObj with the specified number of bins and high knot value.

        :param nb_bins: The number of bins (steps) in the stair object.
        :type nb_bins: int
        :param hiknot: The high knot value beyond which the activation remains constant.
        :type hiknot: float
        """
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
        """
        Initializes member constants of the stair structure.
        """
        self.lowknot = -self.hiknot
        self.dist = self.hiknot - self.lowknot

    def activate_knots(self):
        """
        Calculates and stores the positions of knots and their respective activation levels
        based on the stair structure defined by `nb_bins` and `hiknot`.
        """
        self.binwidth = self.dist / float(self.nb_bins)

        self.knots = [0.0] * self.nb_knots
        self.evalknots = [0.0] * self.nb_knots

        for k in range(self.nb_knots):
            self.knots[k] = self.lowknot + (self.binwidth * float(k))
            self.evalknots[k] = self.activation(self.knots[k])

        self.val_lowknot = self.activation(-1111111111.0)
        self.val_highknot = self.activation(self.hiknot)

    def activation(self, x):
        """
        Computes the activation level for a given input `x` using the sigmoid function.

        :param x: The input value for which to compute the activation.
        :type x: float
        :return: The activation level for `x`.
        :rtype: float
        """
        return self.sigmoid(x)

    def funct(self, x):
        """
        Returns the activation level for a given value `x`, taking into account
        the defined stair structure of the object.

        :param x: The input value for which to find the activation.
        :type x: float
        :return: The activation level for `x`.
        :rtype: float
        """
        if x < self.lowknot:
            return self.val_lowknot
        if x >= self.hiknot:
            return self.val_highknot
        indbin = int((x - self.lowknot) / self.binwidth)
        return self.evalknots[indbin]

    def sigmoid(self, x):
        """
        The sigmoid function, used as the activation function in this stair object.
        Handles overflow by returning 1.0 for high positive inputs and 0.0 for high negative inputs.

        :param x: The input value.
        :type x: float
        :return: The sigmoid activation for `x`.
        :rtype: float
        """
        try:
            return 1.0 / (1.0 + math.exp(-x))
        except OverflowError:
            if x > 0:
                return 1.0
            else:
                return 0.0
