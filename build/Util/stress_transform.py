#coding:utf-8
"次主应力计算/应力转换"
from sympy import symbols,Matrix
import numpy as np
import math



def getTransform():
    '''
    转换后的应力和位移关系
    # Sy'=Sz
    # Sx'=Sx
    # Sz'=Sy
    # Sxy'=-Sxz
    # Syz'=-Syz
    # Sxz'=Sxy
    # Ux'=Ux
    # Uy'=-Uz
    # Uz'=Uy
    '''
    Sx,Sy,Sz,Sxy,Syz,Sxz,Ux,Uy,Uz=symbols("Sx Sy Sz Sxy Syz Sxz Ux Uy Uz")
    S_mat=Matrix([[Sx,Sxy,Sxz],[Sxy,Sy,Syz],[Sxz,Syz,Sz]])
    B_mat=Matrix([[1,0,0],[0,0,-1],[0,1,0]])
    Bt_mat=B_mat.transpose()
    BSBt_mat=B_mat.multiply(S_mat).multiply(Bt_mat)
    U_mat=Matrix([Ux,Uy,Uz]).reshape(3,1)
    UXYZ_mat=B_mat.multiply(U_mat)
    #转换后的应力值
    print BSBt_mat
     #转换后的坐标值
    print UXYZ_mat



def getStressS13(sx,sy,sxy):
    '''
    求平面次主应力
    # Ux'=Ux
    # Uy'=-Uz
    # Uz'=Uy
    '''
    #特征值解法
    stress=np.array([[sx,sxy],[sxy,sy]])
    print sorted(np.linalg.eigvals(stress),reverse=True)
    #材料力学解法
    s1=(sx+sy)/2+math.sqrt((sx-sy)*(sx-sy)/4+sxy*sxy)
    s2=(sx+sy)/2-math.sqrt((sx-sy)*(sx-sy)/4+sxy*sxy)
    print sorted([s1,s2],reverse=True)


getTransform()
############
sx=-85.28
sy=-16.97
sxy=-10.52
getStressS13(sx,sy,sxy)