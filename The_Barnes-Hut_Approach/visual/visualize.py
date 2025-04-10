import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

# Load CSV data
df = pd.read_csv("output.csv")
timesteps = sorted(df["time"].unique())

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
colors = plt.cm.jet(df["body"].nunique())

def update(frame):
    ax.clear()
    ax.set_title(f'Timestep {frame:.2f}')
    data = df[df["time"] == frame]
    for i, row in data.iterrows():
        ax.scatter(row["x"], row["y"], row["z"], color=colors[int(row["body"])])
        ax.text(row["x"], row["y"], row["z"], f'{int(row["body"])}', fontsize=8)
    ax.set_xlim([df["x"].min(), df["x"].max()])
    ax.set_ylim([df["y"].min(), df["y"].max()])
    ax.set_zlim([df["z"].min(), df["z"].max()])

ani = FuncAnimation(fig, update, frames=timesteps, repeat=False)
plt.show()