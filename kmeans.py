"""
Implementation of the K-means algorithm
"""
import math
import copy

class Cluster:
    """
A class representing a cluster in the K-means algorithm.

Attributes:
    cluster_points (list): A list of data points that belong to this cluster.
    centroid (list): The current centroid of the cluster, representing its center.
    convergence (bool): Indicates whether the centroid has converged.

Methods:
    update_centroid():
        Updates the centroid based on the mean of all cluster points.
        
    reset_centroid():
        Resets the cluster by clearing all points while keeping the centroid unchanged.
        
    calculate_distance(point):
        Calculates the distance between the centroid and a given point.
        
    check_convergence(old_centroid):
        Checks if the old centroid is the same as the old centroid, marking convergence if true.
    """


    def __init__(self, centroid=None):
        if centroid is None:
            centroid = []
        self.cluster_points = []
        self.centroid = copy.deepcopy(centroid)
        self.convergence = False

    def update_centroid(self):
        """
        Updates a centroid after an iteration has been run and chekcs for convergence.

        Parameters:
            None.

        Returns:
            Nothing.
        """
        previous_centroid = copy.deepcopy(self.centroid)
        self.reset_centroid()
        self.centroid = [sum(point.values[i] for point in self.cluster_points)/len(self.cluster_points)
                         for i in range(len(self.centroid))]
        self.convergence = self.check_convergence(previous_centroid)

    def reset_centroid(self):
        """
        Sets the centroids values to 0.

        Parameters:
            None.

        Returns:
            Nothing.
        """
        self.centroid = [0 for _ in self.centroid]

    def calculate_distance(self, point):
        """
        Calculates a given points Euclidean distance from the centroid.

        Parameters:
            point(Point): The point for which distance is calculated.

        Returns:
            The Euclidean distance between the point and the centroid.
        """
        return math.sqrt(sum(((self.centroid[i] -
                               point.values[i])**2) for i in range(len(self.centroid))))

    def check_convergence(self, old_centroid):
        """
        Checks for the convergance condition between centroids of subsequent iterations.

        Parameters:
            old_centroid(list): The old centroid to which the new centroid is compared.

        Returns:
            True if convergence is achieved, false otherwise.
        """
        return math.sqrt(sum(((self.centroid[i] - old_centroid[i])**2) 
                            for i in range(len(self.centroid)))) < 0.001

class Point:
    """
A class representing a Euclidean point.

Attributes:
    cluster (Cluster): The cluster to which the point belongs.
    values (list): The coordinates of the point.

Methods:
    None.
    """
    def __init__(self, cluster=None, values=None):
        self.cluster = cluster
        if values is None:
            self.values = []
        self.values = values

def k_means(number_of_clusters, max_iterations, data_points):
    """
    Performs the K-means algorithm until the maximum amount of 
    iterations have been performed or convergance has been reached 
    and returns a 2-dimensional list holding the final clusters centroids.

    Parameters:
        number_of_clusters(int): The number of clusters to use in the algorithm.
        max_iterations(int): The maximum amount of iterations for which to run the algorithm.
        data_points(list): The data points on which to run the algorithm.

    Returns:
        A 2-dimensional list holding the final clusters centroids.
    """
    converted_data_points = convert_to_points(data_points)
    clusters = initialize_clusters(number_of_clusters, converted_data_points)
    iteration_number = 0
    converged = False
    while iteration_number < max_iterations and not converged:
        converged = True
        for point in converted_data_points:
            former_cluster = point.cluster
            if assign_point_to_cluster(point, clusters):
                clusters[former_cluster].cluster_points.remove(point)
        for cluster in clusters:
            cluster.update_centroid()
            if not cluster.convergence and converged:
                converged = False
        iteration_number += 1
    return [cluster.centroid for cluster in clusters]

def convert_to_points(data_points):
    """
    Converts a 2-dimensional list of data points into a list of Point objects.

    Parameters:
        data_points(list): The 2-dimensional list of data points.

    Returns:
        A list of Point instances created from the 2-dimensional data point list.
    """
    return [Point(values=row) for row in data_points]

def initialize_clusters(number_of_clusters, data_points):
    """
    Initializes the cluster instances and adds the data points to the closest ones.

    Parameters:
        number_of_clusters(int): The number of clusters to use in the algorithm.
        data_points(list): The data points on which to run the algorithm.

    Returns:
        The initialized clusters.
    """
    clusters = [Cluster(data_points[i].values) for i in range(number_of_clusters)]
    for i in range(number_of_clusters):
        clusters[i].cluster_points.append(data_points[i])
        data_points[i].cluster = i
    for point in data_points[number_of_clusters:]:
        assign_point_to_cluster(point, clusters)
    for cluster in clusters:
        cluster.update_centroid()
    return clusters

def assign_point_to_cluster(point, clusters):
    """
    Assigns the given point to its closest cluster.

    Parameters:
        point(Point): The point to be added to a cluster.
        clusters(list): The list of current clusters.

    Returns:
        True if the points cluster changed, false otherwise.
    """
    min_cluster = min(((cluster ,cluster.calculate_distance(point))
                    for cluster in clusters), key = lambda x: x[1])[0]
    if min_cluster != point.cluster:
        min_cluster.cluster_points.append(point)
        point.cluster = min_cluster
        return True
    return False
