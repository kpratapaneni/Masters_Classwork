from math import cos, sin, pi, sqrt
import pcl
import numpy as np

point_details = []

def cartesian(lattitude,longitude, elevation):
    rad=6378137.0+elevation
    foc=1.0/298.257224
    cos_Lat=cos(lattitude*pi/180.0)
    sin_Lat=sin(lattitude*pi/180.0)
    cos_Lon=cos(longitude*pi/180.0)
    sin_Lon=sin(longitude*pi/180.0)
    C=1.0/sqrt(cos_Lat*cos_Lat+(1-foc)*(1-foc)*sin_Lat*sin_Lat)
    S=(1.0-foc)*(1.0-foc)*C
    x=(rad*C+0.0)*cos_Lat*cos_Lon
    y=(rad*C+0.0)*cos_Lat*sin_Lon
    z=(rad*S+0.0)*sin_Lat
    return x,y,z

def writeOriginalCloudDataIntoFile():
    original = open('Original Point Cloud.obj', 'w')
    fusefile = open('final_project_point_cloud.fuse', 'rb')
    for line in fusefile:
        pts=[]
        sp=line.strip().split(' ')
        x,y,z=cartesian(float(sp[0]), float(sp[1]), float(sp[2]))
        pts.append(x)
        pts.append(y)
        pts.append(z)
        point_details.append(pts)
        line="v "+str(x)+" "+str(y)+" "+str(z)
        original.write(line)
        original.write("\n")
    original.close()
    originalPointCloudData(point_details)
    
def originalPointCloudData(pd):
    pd=np.array(pd,dtype=np.float32)
    pl=pcl.PointCloud()
    pl.from_array(pd)
    print("Original point cloud Data")
    print(pl)
    filteredDatawoOutliers(pl)

def filteredDatawoOutliers(p):
    filt=p.make_statistical_outlier_filter()
    filt.set_mean_k(50)
    filt.set_std_dev_mul_thresh(5.0)
    filt.set_negative(False)
    fc=filt.filter()
    print("Filtered data not including outliers")
    print(fc)
    filteredwoLargeComponents(fc)

def filteredwoLargeComponents(fc):
    IF1 = open('Intermediate_File1.obj', 'w')
    for point in fc:
        line="v "+str(point[0])+" "+str(point[1])+" "+str(point[2])
        IF1.write(line)
        IF1.write("\n")
    IF1.close()
    kd=fc.make_kdtree_flann()
    ind,sqr_dist=kd.nearest_k_search_for_cloud(fc, 1000)
    distances=np.sum(sqr_dist, axis=1)
    remove_indices=[]
    for i in xrange(np.shape(distances)[0]):
        if distances[i]<5000.0:
            remove_indices.extend(ind[i])
    remove_unique_indices=set(remove_indices)
    fc=fc.extract(remove_unique_indices, negative=True)
    print("Filtered  data not including large components")
    print(fc)
    IF2=open('Intermediate_File2.obj', 'w')
    for point in fc:
        line="v "+str(point[0])+" "+str(point[1])+" "+str(point[2])
        IF2.write(line)
        IF2.write("\n")
    IF2.close()
    cylindricalSegmentation(fc)
    
def cylindricalSegmentation(fc):
    Seg = fc.make_segmenter_normals(ksearch=50)
    Seg.set_optimize_coefficients(True)
    Seg.set_model_type(pcl.SACMODEL_CYLINDER)
    Seg.set_normal_distance_weight(0.1)
    Seg.set_method_type(pcl.SAC_RANSAC)
    Seg.set_max_iterations(1000)
    Seg.set_distance_threshold(20)
    Seg.set_radius_limits(0, 10)
    segment_indices, model = Seg.segment()
    fc = fc.extract(segment_indices, negative=False)
    print ("Cloud Data Points after cylindrical segmentation")
    print (fc)
    finalCloudData(fc)

def finalCloudData(fc):
    Seg=fc.make_segmenter_normals(ksearch=50)
    Seg.set_optimize_coefficients(True)
    Seg.set_model_type(pcl.SACMODEL_PLANE)
    Seg.set_method_type(pcl.SAC_RANSAC)
    Seg.set_normal_distance_weight(0.1)
    Seg.set_distance_threshold(85)
    Seg.set_max_iterations(100)
    indices,model=Seg.segment()
    fc=fc.extract(indices,negative=True)
    filt=fc.make_passthrough_filter()
    filt.set_filter_field_name("x")
    filt.set_filter_limits(0,4364071.0)
    fc=filt.filter()
    print("Final point cloud Data")
    print(fc)
    final_output=open('Final_File.obj', 'w')
    for pts in fc:
        ln="v "+str(pts[0])+" "+str(pts[1])+" "+str(pts[2])
        final_output.write(ln)
        final_output.write("\n")
    final_output.close()
    
writeOriginalCloudDataIntoFile()



