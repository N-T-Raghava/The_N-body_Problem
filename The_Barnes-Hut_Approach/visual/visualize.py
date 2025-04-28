import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

# Load CSV data
df = pd.read_csv("D:/SOFTWARE_PROJECT/Sweet_16/The_N-body_Problem/The_Barnes-Hut_Approach/visual/output.csv")
timesteps = sorted(df["time"].unique())

# Setup figure and 3D axes
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Generate a color for each body
num_bodies = df["body"].nunique()
colors = plt.cm.jet(np.linspace(0, 1, num_bodies))

# Correct body ID to color mapping
body_ids = sorted(df["body"].unique())
body_id_to_color = {body_id: colors[i] for i, body_id in enumerate(body_ids)}

# Precompute axis limits
xmin, xmax = df["x"].min(), df["x"].max()
ymin, ymax = df["y"].min(), df["y"].max()
zmin, zmax = df["z"].min(), df["z"].max()
if abs(zmax - zmin) < 1e-6:
    zmin -= 1
    zmax += 1

def update(frame):
    ax.clear()
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)
    ax.set_zlim(zmin, zmax)
    ax.set_title(f'Timestep {frame:.2f}')

    data = df[df["time"] == frame]
    for _, row in data.iterrows():
        ax.scatter(row["x"], row["y"], row["z"], color=body_id_to_color[int(row["body"])])
        ax.text(row["x"], row["y"], row["z"], str(int(row["body"])), fontsize=8)

# Create animation
ani = FuncAnimation(fig, update, frames=timesteps, repeat=False)

# Save the animation as a gif
ani.save('animation.gif', writer='pillow', fps=10)

# plt.show()   # If you want live preview
