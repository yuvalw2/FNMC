# -*- coding: utf-8 -*-
"""
Created on Tue Sep 28 16:24:51 2021

@author: Yuval Was
"""
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
%matplotlib qt
N=4
activity=1
c_names=["Time Gamma","Energy Gamma", "Id Gamma","Time Neutron","Energy Neutron","Id Neutron"]
df1 = pd.read_csv('rossi_nt_rossi_t0.csv',sep=',',   header=9, names=c_names)
df2 = pd.read_csv('rossi_nt_rossi_t1.csv',sep=',',   header=9, names=c_names)
df=pd.concat([df1,df2])
t0=0
pulses_df=pd.DataFrame([],columns=['Time (s)','IsNeutron'])

#19.85 terabecquerels per gram
rate=595500 #1 microgram fission rates per sec

def merge_pulses(indices,times,window,energies,thresh):
    pulses=dict()
    times=np.array(list(map(float,times)))
    energies=np.array(list(map(float,energies)))
    indices=np.array(list(map(int,indices)))
    
    
    for i in range(N):
        pulses[i]=np.vstack((times[indices==i], energies[indices==i])).T
        pulses[i]=pulses[i][np.argsort(pulses[i][:,-1])]
    
    
    for key in pulses.keys():
        temp_time=[]
        temp_energy=[]
        det_pulses=pulses[key]
        for i in range(det_pulses.shape[0]):
            if i==0:
                temp_time.append(det_pulses[0,0])
                temp_energy.append(det_pulses[0,1])
            elif (det_pulses[i,0]-temp_time[-1])>window:
                temp_time.append(det_pulses[i,0])
                temp_energy.append(det_pulses[i,1])
            else:
                temp_energy[-1]+=det_pulses[i,1]
        pulses[key]=np.vstack((temp_time, temp_energy)).T
        pulses[key]=pulses[key][np.argsort(pulses[key][:,-1])]     
        pulses[key]=pulses[key][pulses[key][:,1]>thresh]
        """
        print(pulses[key])
        print("-----")
    print("-@@-@@-@@-@@-@@-")"""
    return pulses

    

particles_names=["Gamma","Neutron"]
energy_threshold=[0.03,0.3]
t_global=0;

final_pulses_times=[]
final_pulses_isN=[]
for i in range(df.shape[0]):
    t_global+=np.random.exponential(1/rate);
    for particle_i in range(0,2):
        particle_name=particles_names[particle_i]
        row=df.iloc[i]
        time=row["Time "+particle_name]
        if isinstance(time,str):
            energy=row["Energy "+particle_name].split(";")
            det_index=row["Id "+particle_name].split(";")
            time=row["Time "+particle_name].split(";")
            final_pulses=merge_pulses(det_index,time,1e-9,energy,energy_threshold[particle_i])
            
            for key, item in final_pulses.items():
                if (item.size>0):
                    time_row=item[:,0]+t_global
                    final_pulses_times+=list(time_row)
                    final_pulses_isN+=[particle_name=="Neutron" for x in time_row]
                
final_pulses_times=np.array(final_pulses_times)
final_pulses_isN=np.array(final_pulses_isN)

plt.plot(final_pulses_times,final_pulses_isN)
plt.show()
            
            
            
    
    
    