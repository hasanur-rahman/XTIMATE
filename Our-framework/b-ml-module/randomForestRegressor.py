from sklearn.model_selection import train_test_split
import pandas as pd
import numpy as np
import os, sys, csv
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import GridSearchCV
# from sklearn.model_selection import RandomizedSearchCV
# from sklearn.model_selection import RepeatedKFold
# from sklearn.model_selection import cross_val_score
from sklearn.ensemble import RandomForestRegressor
# from sklearn.model_selection import RepeatedKFold
# from sklearn.model_selection import RepeatedStratifiedKFold
# from sklearn.model_selection import cross_val_score
# from sklearn.feature_selection import RFE
# from sklearn.model_selection import KFold
# from sklearn.metrics import make_scorer
import math
from math import sqrt
from datetime import datetime
import time


EPSILON=0.000000001

inp_train_file = str(sys.argv[1])
inp_test_file = str(sys.argv[2])
outputPath = str(sys.argv[3])


trainingCost = 0.0
inferenceCost = 0.0

columns = ["name", "data_dispersiveness", "data_smoothness",  "sobel_kernel_entropy", "error_bound", "cratio"]

# sobel
dropped_columns = ["name"]




required_columns = []

for column in columns:
	if column not in dropped_columns:
		required_columns.append(column)



out_train_file = "temp_train_with_header" + ".csv"
out_test_file = "temp_test_with_header" + ".csv"
cmd_buffer = "rm " + out_train_file + " " + out_test_file
os.system(cmd_buffer)

csv_train_out = open(out_train_file, 'w')
csvwriter = csv.writer(csv_train_out)

csvwriter.writerow(columns)
with open(inp_train_file, 'r') as csv_file:
    csv_reader = csv.reader(csv_file)
    for row in csv_reader:
        csvwriter.writerow(row)


csv_train_out.close()

df = pd.read_csv(out_train_file)

df = df.drop(columns=dropped_columns).copy()

for column in required_columns:
	df[column] = df[column].astype('float64')

X_train = df.drop(columns=["cratio"]).copy()
y_train = df["cratio"]  


csv_test_out = open(out_test_file, 'w')
csvwriter = csv.writer(csv_test_out)


csvwriter.writerow(columns)
#
with open(inp_test_file, 'r') as csv_file:
    csv_reader = csv.reader(csv_file)
    for row in csv_reader:
        csvwriter.writerow(row)


csv_test_out.close()


df = pd.read_csv(out_test_file)
testNames = df["name"].values.flatten()


testErrorBounds = df["error_bound"]
testErrorBounds = testErrorBounds.values.flatten()


df = df.drop(columns=dropped_columns).copy()
for column in required_columns:
	df[column] = df[column].astype('float64')

# print("Test info: ", df.info())
X_test = df.drop(columns=["cratio"]).copy()
y_test = df["cratio"]


start = time.time()
scaler = StandardScaler()
train_scaled = scaler.fit_transform(X_train)

parameter_space = {'n_estimators': [50,100,500,1000],
               'max_features': ['auto'],
               'min_samples_leaf': [1, 2, 4],
               'bootstrap': [True, False],
               'random_state': [1]}



model = RandomForestRegressor()
grid = GridSearchCV(model, parameter_space, n_jobs=1, cv=5, verbose=3)
grid.fit(train_scaled, y_train)
end = time.time()
print("Total Training time: ", (end - start), " sec")
trainingCost = (end - start)



########




start = time.time()
test_scaled = scaler.transform(X_test)
y_pred = grid.predict(test_scaled)
y_test_np = y_test.values.flatten()
y_pred = y_pred.flatten()
end = time.time()
print("Number of testing samples: ", y_test_np.size)
print("Total Inference time: ", (end - start), " sec")
print("Average Inference time: ", (end - start)/y_test_np.size, " sec")
inferenceCost = (end - start)
##########




os.system("mkdir %s"%outputPath)
buffer = "%s/ml_model_evaluation_results"%outputPath + ".csv"
csv_pred_out = open(buffer, 'w')
csvwriter = csv.writer(csv_pred_out)


# avg_estimate_error = 0.0
# it=0


# for test_val in y_test_np:
#     trueValue = float(y_test_np[it])
#     predValue = float(y_pred[it])
#     avg_estimate_error += (100.0 * math.fabs(trueValue - predValue)) / trueValue
#     csvwriter.writerow([testNames[it], testErrorBounds[it], trueValue, predValue, (100.0 * math.fabs(trueValue - predValue)) / trueValue])
#     it = it + 1



# avg_estimate_error = avg_estimate_error / (1.0 * it)

# print(", Average estimation error: ", avg_estimate_error, ", Features: ", X_test.columns.values)

it=0

for test_val in y_test_np:
    predValue = float(y_pred[it])
    trueValue = float(y_test_np[it])
    csvwriter.writerow([testNames[it], testErrorBounds[it], predValue])
    it = it + 1


csv_pred_out.close()

buffer = "%s/ml_model_performance"%outputPath + ".log"
csv_pred_out = open(buffer, 'w')
csv_pred_out.write("Total training time: %.5f\n"%(trainingCost))
csv_pred_out.write("Total inference time: %.5f\n"%(inferenceCost))
csv_pred_out.write("Average inference time: %.5f\n"%(inferenceCost/y_test_np.size))
csv_pred_out.close()
os.system(cmd_buffer)

