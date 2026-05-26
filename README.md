# urdf-closed-chain-links

URDF cannot directly represent closed-chain mechanisms because a child link cannot have multiple parent links. This project explores workaround methods for simulating closed-chain gripper mechanisms in Gazebo.

URDF files follow a tree-like structure where links act as nodes and joints act as the branches connecting them. In this hierarchy, each link can only have one parent link, which prevents direct implementation of closed-chain mechanisms.

In this project, I will experiment with possible workaround methods for this limitation.

## Step 1

For this project, I am using a gripper design referenced from an existing project. I simplified the CAD model and created a basic URDF representation of the mechanism by leaving the kinematic loops open, resulting in multiple independent branches.
