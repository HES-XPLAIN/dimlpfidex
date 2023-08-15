# -*- coding: utf-8 -*-
"""
Created on Mon Mar  8 12:38:31 2021

@author: guido.bologna
"""

import numpy as np
np.random.seed(seed=None)

import sys

from sklearn.ensemble import RandomForestClassifier

from sklearn.tree import DecisionTreeClassifier
from sklearn.tree import _tree


##############################################################################

def tree_to_code(tree):
    tree_ = tree.tree_
    feature_name = [
        str(i+1) if i != _tree.TREE_UNDEFINED else "undefined!"
        for i in tree_.feature
    ]
    pathto=dict()

    global k
    k = 0
    def recurse(node, depth, parent):
        global k
        indent = "  " * depth

        if tree_.feature[node] != _tree.TREE_UNDEFINED:
            name = feature_name[node]
            threshold = tree_.threshold[node]
            s= "{} <= {} ".format( name, threshold, node )
            if node == 0:
                pathto[node]=s
            else:
                pathto[node]=pathto[parent]+' & ' +s

            recurse(tree_.children_left[node], depth + 1, node)
            s="{} > {}".format( name, threshold)
            if node == 0:
                pathto[node]=s
            else:
                pathto[node]=pathto[parent]+' & ' +s
            recurse(tree_.children_right[node], depth + 1, node)
        else:
            k=k+1
            print(k,')', pathto[parent], tree_.value[node])
    recurse(0, 1, 0)


##############################################################################

def loadData(oneFile, oneFileClass):

   print("Loading data...")

   data       = np.loadtxt(oneFile)
   dataClass  = np.loadtxt(oneFileClass)
   dataClass2 = dataClass[:, 1]

   return (data, dataClass2)

##############################################################################

def crossVal(r, f):

   trStr               = 'TD.' + str(r) + '.' + str(f)
   trClsStr            = 'TL.' + str(r) + '.' + str(f)
   (train, trainClass) = loadData(trStr, trClsStr)

   tstStr               = 'CD.' + str(r) + '.' + str(f)
   tstClsStr            = 'CL.' + str(r) + '.' + str(f)
   (test, testClass)    = loadData(tstStr, tstClsStr)

   nbTrees = 150

   clf = RandomForestClassifier(n_estimators=nbTrees, verbose=1, oob_score=False)

# clf = RandomForestClassifier(n_estimators=nbTrees, verbose=1, oob_score=true, n_jobs=1, random_state=1)

   model = clf.fit(train, trainClass)

   print('Train accuracy : ', model.score(train, trainClass))
   print('Test accuracy : ', model.score(test, testClass))

   original_stdout = sys.stdout # Save a reference to the original standard output

   with open('rfRules.' + str(r) + '.' + str(f), 'w') as f:

      sys.stdout = f # Change the standard output to the file we created.

      for i in range(nbTrees):
          tree_to_code(model.estimators_[i])

      sys.stdout = original_stdout

##############################################################################

for i in range(10):
    for j in range(10):
        crossVal(i+1, j+1)

# crossVal(1, 3)
