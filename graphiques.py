from math import exp
"""
for i in range(100, 1000, 100):
    for j in range(11):
        a=(0.641*i)/(1+0.0569*(i*0.01*j)**1.301)
        print(f"auto {i}, danger {0.01*j} : T* = {a}")

print("---------------------------------")

for i in range(100, 1000, 100):
    for j in range(11):
        a=(0.959*(i**0.936)) / ((1+0.01576*(i**1.228)*j*0.01)**1.504)
        print(f"auto {i}, danger {0.01*j} : T* = {a}")

print("---------------------------------")

for i in range(100, 1000, 100):
    for j in range(11):
        a=0.60*i*exp(-0.12*(i**0.55)*j*0.01)
        print(f"auto {i}, danger {0.01*j} : T* = {a}")


print("---------------------------------")
""""""
for i in range(100, 1000, 100):
    for j in range(101):
        a=0.945*(i**0.941)*exp(-0.0613*(i**0.840)*((j*0.01)**0.691))
        print(f"auto {i}, danger {0.01*j} : T* = {a}")

for i in range(100, 1000, 100):
    for j in range(11):
        a=(0.62*i)/((1+0.020*(i**2)*((j*0.01)**2))**0.53)
        print(f"auto {i}, danger {0.01*j} : T* = {a}")

print("---------------------------------")

for i in range(100, 1000, 100):
    for j in range(11):
        a=(0.3534*i**1.091)/((1+0.020*(i**2)*((j*0.01)**2))**0.53)
        print(f"auto {i}, danger {0.01*j} : T* = {a}")


print("---------------------------------")

for j in range(11):
    a=(0.3534*500**1.091)/((1+0.020*(500**2)*((j*0.01)**2))**0.53)
    print(f"auto {i}, danger {0.01*j} : T* = {a}")

"""
import io
import numpy as np
import pandas as pd
import plotly.graph_objects as go

# ==========================================
# 1. PRÉPARATION DES DONNÉES (Modèle & Données)
# ==========================================

# Définition de la fonction du modèle mathématique
def modele_t_star(A, D):
    num = 0.3534 * (A**1.091)
    den = (1 + 5000 * (D**2))**0.53
    return num / den

# Données expérimentales fournies
data_str = """autonomie,danger,topt_moyen
100,0.000,55.75
100,0.010,55.00
100,0.020,55.50
100,0.030,54.75
100,0.040,54.50
100,0.050,55.00
100,0.060,51.75
100,0.070,49.25
100,0.080,50.25
100,0.090,45.50
100,0.100,43.50
200,0.000,116.25
200,0.010,116.50
200,0.020,115.75
200,0.030,101.00
200,0.040,99.00
200,0.050,72.00
200,0.060,65.25
200,0.070,50.25
200,0.080,48.50
200,0.090,42.50
200,0.100,40.50
300,0.000,179.00
300,0.010,178.25
300,0.020,140.75
300,0.030,109.75
300,0.040,90.25
300,0.050,78.50
300,0.060,60.00
300,0.070,50.25
300,0.080,48.25
300,0.090,39.00
300,0.100,38.00
400,0.000,248.00
400,0.010,219.75
400,0.020,164.75
400,0.030,109.75
400,0.040,93.25
400,0.050,65.75
400,0.060,58.75
400,0.070,46.75
400,0.080,43.50
400,0.090,39.75
400,0.100,39.50
500,0.000,314.75
500,0.010,244.75
500,0.020,146.25
500,0.030,116.75
500,0.040,93.00
500,0.050,63.00
500,0.060,63.50
500,0.070,44.75
500,0.080,42.50
500,0.090,38.50
500,0.100,40.25
600,0.000,380.50
600,0.010,264.75
600,0.020,158.25
600,0.030,112.50
600,0.040,80.50
600,0.050,64.50
600,0.060,56.25
600,0.070,39.00
600,0.080,43.50
600,0.090,33.75
600,0.100,38.00
700,0.000,451.50
700,0.010,230.25
700,0.020,153.00
700,0.030,112.00
700,0.040,87.50
700,0.050,62.50
700,0.060,57.25
700,0.070,39.00
700,0.080,39.25
700,0.090,32.50
700,0.100,38.00
800,0.000,519.25
800,0.010,239.50
800,0.020,164.25
800,0.030,110.00
800,0.040,76.50
800,0.050,58.75
800,0.060,56.75
800,0.070,36.50
800,0.080,42.00
800,0.090,34.50
800,0.100,37.50"""

df = pd.read_csv(io.StringIO(data_str))

# Configuration de l'angle de vue de la caméra
camera_view = dict(
    eye=dict(x=-1.8, y=-1.4, z=0.8)
)

# Palette de couleurs personnalisée
custom_colorscale = [
    [0.0, 'rgb(10, 60, 120)'],
    [0.3, 'rgb(40, 130, 220)'],
    [0.6, 'rgb(40, 180, 130)'],
    [0.8, 'rgb(200, 160, 60)'],
    [1.0, 'rgb(230, 100, 40)']
]

# ==========================================
# GRAPHIQUE 1 : LE MODÈLE (Surface continue)
# ==========================================

A_vals = np.linspace(100, 800, 100)
D_vals = np.linspace(0.00, 0.10, 100)
A_grid, D_grid = np.meshgrid(A_vals, D_vals)
T_model = modele_t_star(A_grid, D_grid)

fig1 = go.Figure(data=[go.Surface(
    x=D_grid, 
    y=A_grid, 
    z=T_model, 
    colorscale=custom_colorscale,
    colorbar=dict(
        title=dict(text="T*", font=dict(color='black')),
        tickfont=dict(color='black')
    )
)])

fig1.update_layout(
    title="Modèle : T*(A, D)",
    template="plotly_white",
    scene=dict(
        xaxis=dict(title="Danger (D)", range=[0.10, 0.00]),
        yaxis=dict(title="Autonomie"),
        zaxis=dict(title="T*", range=[0, 700]),
    ),
    scene_camera=camera_view,
    margin=dict(l=0, r=0, b=0, t=40)
)

fig1.show()


# ==========================================
# GRAPHIQUE 2 : LES DONNÉES EXPERIMENTALES
# ==========================================

df_pivot = df.pivot(index='autonomie', columns='danger', values='topt_moyen')

fig2 = go.Figure(data=[go.Surface(
    x=df_pivot.columns.values, 
    y=df_pivot.index.values, 
    z=df_pivot.values, 
    colorscale=custom_colorscale,
    colorbar=dict(
        title=dict(text="T*", font=dict(color='black')),
        tickfont=dict(color='black')
    )
)])

fig2.update_layout(
    title="Données Expérimentales : T*(A, D)",
    template="plotly_white",
    scene=dict(
        xaxis=dict(title="Danger (D)", range=[0.10, 0.00]),
        yaxis=dict(title="Autonomie"),
        zaxis=dict(title="T*", range=[0, 700]),
    ),
    scene_camera=camera_view,
    margin=dict(l=0, r=0, b=0, t=40)
)

fig2.show()
