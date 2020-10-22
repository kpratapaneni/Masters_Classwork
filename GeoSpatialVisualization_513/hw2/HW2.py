import math
from collections import defaultdict

#Global variables used for the code (collection defaultdict is used to handle error in list)
pointDataList = defaultdict(list)
linkDataList = defaultdict(list)

#Finding the latitude and longitude of fiven points
class findLatLong():
    def __init__(self, shapeInfo):
        self.ID = shapeInfo
        shape_attributes = shapeInfo.split("/")
        self.longitude, self.latitude = map(float, (shape_attributes[0], shape_attributes[1]))

#Packing LinkData.csv into a package object        
class PackageLinkID():
    def __init__(self, ID, start, end):
        self.id = ID
        self.point1, self.point2 = findLatLong(start), findLatLong(end)
        self.longitude_vector, self.latitude_vector = self.point2.longitude - self.point1.longitude, self.point2.latitude - self.point1.latitude
        self.length = math.sqrt(self.longitude_vector ** 2 + self.latitude_vector ** 2)
        if self.latitude_vector != 0:
            self.radian = math.atan(self.longitude_vector / self.latitude_vector)
        elif self.longitude_vector > 0:
            self.radian = math.pi / 2
        else:
            self.radian = math.pi * 3 / 2
            
    #Calculating the distance between a packed linkData and a new probe-point
    def Distance(self, point):
        target_longitude, target_latitude = point.longitude - self.point1.longitude, point.latitude - self.point1.latitude
        dist_point_refnode = (target_longitude ** 2) + (target_latitude ** 2)
        projection = (target_longitude * self.longitude_vector + target_latitude * self.latitude_vector) / self.length
        if projection < 0:
            return dist_point_refnode
        if projection ** 2 > self.length ** 2:
            return (point.longitude - self.point2.longitude) ** 2 + (point.latitude - self.point2.latitude) ** 2
        return (target_longitude**2 + target_latitude**2) - projection**2

   #Calculating the distance between two links using link data. 
    def DistanceFromLink(self, point):
        target_longitude, target_latitude = point.longitude - self.point1.longitude, point.latitude - self.point1.latitude
        return math.sqrt(target_longitude**2 + target_latitude**2)

#Class used to package the sample ID data set into an object    
class PackageSampleID(object):
    def __init__(self, line):
        self.sampleID,self.dateTime,self.sourceCode,self.latitude,self.longitude,self.altitude,self.speed,self.heading = line.strip().split(',')
        self.direction = ""
        self.linkID = None
        self.distanceFromReference = None
        self.distanceFromLink = None
        self.slope = None

    # finding the direction 
    def getDirection(self, A, B):
        self.direction = "F" if ((math.cos(A) * math.cos(B) + math.sin(A) * math.sin(B)) > 0) else "T"

    #Converting object to string of required format
    def toString(self):
        return '{}, {}, {}, {}, {}, {}, {}, {}, {}, {} ,{}, {}\n' \
            .format(self.sampleID, self.dateTime,self.sourceCode,self.latitude,self.longitude,self.altitude,self.speed,self.heading,
                    self.linkID,self.direction,self.distanceFromReference,self.distanceFromLink)

class SlopeProbeData(object):
    def __init__(self, line):
        self.sampleId, self.dateTime, self.sourceCode, self.latitude, self.longitude, self.altitude, self.speed, self.heading, \
        self.linkId, self.direction , self.distanceFromReference, self.distanceFromLink = line.split(',')        
        self.elevation = None
        self.slope = None

   #converting data into comma seperated string
    def toString(self):       
        return '{}, {}, {}, {}, {}, {}, {}, {}, {}, {} , {}, {}\n' \
            .format(self.sampleId, self.dateTime, self.sourceCode, self.latitude, self.longitude, self.altitude, self.speed, self.heading,
                    self.linkId, self.direction, self.distanceFromReference, self.slope)

#Reading all attributes of slop data into a class
class SlopeLinkData(object):
    def __init__(self, line):
        self.linkId, self.refNodeId, self.nrefNodeId, self.length, self.funClass, self.travelDirection, self.speedCategory, self.fromRefSpeedLimit,\
        self.toRefSpeedLimit, self.fromRefNumLanes, self.toRefNumLanes, self.multiDigitized, self.urban, self.timezone, self.shapeInfo, self.curvatureInfo,\
        self.slopeInfo = line.strip().split(',')
        self.referenceNodeLatitude, self.ReferenceNodeLongitude,_  = self.shapeInfo.split('|')[0].split('/')
        self.referenceNode = map(float, (self.referenceNodeLatitude, self.ReferenceNodeLongitude))
        self.probePoints = []
        
#Read the linkData from the csv and create linkDataList/pointDataList           
def readLinkData():
    print("Reading LinkData")
    with open("Partition6467LinkData.csv") as f:
        for line in f:
            col = line.strip().split(",")
            shi = col[14].split("|")
            silen = len(shi)-1
            for i in range(0,silen):
                tmpSh = PackageLinkID(col[0], shi[i], shi[i+1])
                linkDataList[col[0]].append(tmpSh)
                pointDataList[shi[i]].append(tmpSh)
                pointDataList[shi[i + 1]].append(tmpSh)
            # print(pointDataList)
            # print(linkDataList)

#Matching the linkDataList with the probe data, find the shortest distance and create output file Partition6467MatchedPoints.csv
def matchPoints():
    matchedPoints = open("Partition6467MatchedPoints.csv", "w+")
    previousID = None
    matchingArray = []
    print("Writing matched points into Partition6467MatchedPoints.csv.....please wait ");
    records=0;
    for line in open("Partition6467ProbePoints.csv").readlines():
        if records < 5500:
            records=records+1;
            probePoints = PackageSampleID(line)
            latitude_longitude = findLatLong(probePoints.latitude + "/" + probePoints.longitude)
            #Check if the previous value is repeated
            if probePoints.sampleID != previousID:
                previousID = probePoints.sampleID
                for key in linkDataList.keys():
                    for link in linkDataList[key]:
                        distance = link.Distance(latitude_longitude)
                        #If the probe point is empty or less than the distance find the direction b/w the point and the linkdata
                        if not probePoints.distanceFromReference or distance < probePoints.distanceFromReference:
                            probePoints.distanceFromReference, probePoints.linkID = distance, link.id
                            probePoints.distanceFromLink = linkDataList[probePoints.linkID][0].DistanceFromLink(latitude_longitude)
                            probePoints.getDirection(float(probePoints.heading), link.radian)
                            matchingArray = [link.point1, link.point2]
           
            else:
                #Looping through the array of match data when the repeation occurs
                for candidate_point in matchingArray:
                    for link in pointDataList[candidate_point.ID]:
                        distance = link.Distance(latitude_longitude)
                        if not probePoints.distanceFromReference or distance < probePoints.distanceFromReference:
                            probePoints.distanceFromReference, probePoints.linkID = distance, link.id
                            probePoints.distanceFromLink = linkDataList[probePoints.linkID][0].DistanceFromLink(latitude_longitude)
                            probePoints.getDirection(float(probePoints.heading), link.radian)
        else:
            break;     
        #Finding the distance from the reference 
        probePoints.distanceFromReference = math.sqrt(probePoints.distanceFromReference) * (math.pi / 180 * 6371000)
        #Finding the distance from the link
        probePoints.distanceFromLink = probePoints.distanceFromLink * (math.pi / 180 * 6371000)
        matchedPoints.write(probePoints.toString())

    matchedPoints.close()
    print("mapmatching is completed");

# Changes done
def distance(start_longitude, start_latitude, end_longitude, end_latitude):
    start_longitude, start_latitude, end_longitude, end_latitude = list(map(math.radians, [start_longitude, start_latitude, end_longitude, end_latitude]))
    long_distance, lat_distance = end_longitude - start_longitude , end_latitude - start_latitude
    distance = math.sin(lat_distance/2)**2 + math.cos(start_latitude) * math.cos(end_latitude) * math.sin(long_distance/2)**2
    distance = 6371 * 2 * math.asin(math.sqrt(distance))
    return distance

# TODO CHANGES
def readSlopeDataIntoArr():
    slopeArr = []
    for line in open("Partition6467LinkData.csv").readlines():
        slopeArr.append(SlopeLinkData(line))
    return slopeArr

def addProbePoints(slopeArr, curr_probe_data):
    for link in slopeArr:
        if link.slopeInfo != '' and link.linkId.strip() == curr_probe_data.linkId.strip():
            link.probePoints.append(curr_probe_data)
            break

def calcSlope(curr_probe_data, prev_probe_data):
    try:
        start, end = list(map(float, [curr_probe_data.longitude, curr_probe_data.latitude])), list(map(float, [prev_probe_data.longitude, prev_probe_data.latitude]))
        opp_angle = float(curr_probe_data.altitude) - float(prev_probe_data.altitude)
        hyp_angle = distance(start[0], start[1], end[0], end[1]) / 1000
        curr_probe_data.slope = (2 * math.pi * math.atan(opp_angle / hyp_angle)) / 360
    except ZeroDivisionError:
        curr_probe_data.slope = 0.0
    except:
        curr_probe_data.slope = 0.0

def calcAndEvalSlopeData():
    print("Computing and analysing the slope data!!!!")
    slopeArr = readSlopeDataIntoArr();

    print("Computing road slope from given data Partition6467LinkData.csv started!!!!!")
    road_slope_file = open("RoadSlopeData.csv", 'w')
    noOfRecords = 0
    prev_probe_data = None
    with open("Partition6467MatchedPoints.csv") as data_file:
        for line in data_file:
            if noOfRecords < 5000:
                curr_probe_data = SlopeProbeData(line)
                if not prev_probe_data or curr_probe_data.linkId != prev_probe_data.linkId:
                    curr_probe_data.slope = ''
                else:
                    calcSlope(curr_probe_data, prev_probe_data)
                    addProbePoints(slopeArr, curr_probe_data)
                prev_probe_data = curr_probe_data
                road_slope_file.write(curr_probe_data.toString())
            else:
                break;
            noOfRecords += 1;
        road_slope_file.close()
    print("Computing road slope from given data Partition6467LinkData.csv done !!!!!")
    print("Analysing the probe data slope from the computed data is started !!!!")

    slope_eval_csv_file = open("ProbeEvaluationData.csv", 'w')
    for dataPoint in slopeArr:
        if len(dataPoint.probePoints) > 0:
            sum = 0.0
            dataSlopeArr = dataPoint.slopeInfo.strip().split('|')
            for currSlope in dataSlopeArr:
                sum += float(currSlope.strip().split('/')[1])
            slope = sum / len(dataSlopeArr)
            finalSum, probeCount = 0.0, 0
            for eachProbe in dataPoint.probePoints:
                if eachProbe.direction == "T":
                        eachProbe.slope = -eachProbe.slope
                if eachProbe.slope != '' and eachProbe.slope != 0:
                    finalSum += eachProbe.slope
                    probeCount += 1

            evaluatedSlope = 0
            if(probeCount != 0):
                evaluatedSlope = finalSum / probeCount
            slope_eval_csv_file.write('{}, {}, {}\n'.format(dataPoint.linkId, slope, evaluatedSlope))
    slope_eval_csv_file.close()
    print("Analysing the probe data slope from the computed data is done !!!!")

if __name__ == '__main__':
    #Function to create linkdata array
    readLinkData()
    #Function to create the MatchedPoints csv
    matchPoints()

    # Calculate and evaluate the slope
    calcAndEvalSlopeData();