# urdf-closed-chain-links

URDF cannot directly represent closed-chain mechanisms because a child link cannot have multiple parent links. This project explores workaround methods for simulating closed-chain gripper mechanisms in Gazebo.

URDF files follow a tree-like structure where links act as nodes and joints act as the branches connecting them. In this hierarchy, each link can only have one parent link, which prevents direct implementation of closed-chain mechanisms.

In this project, I will experiment with possible workaround methods for this limitation.

## Step 1

For this project, I am using a gripper design referenced from an existing project. I simplified the CAD model and created a basic URDF representation of the mechanism by leaving the kinematic loops open, resulting in multiple independent branches.

## Step 2:

The first branch of the gripper was connected to a position controller through ros2_control. Individual joint motion was successfully tested in Gazebo, confirming that the simulation, controller configuration, and hardware interfaces were functioning correctly.

## Step 3

The next step is to investigate an approach for reproducing closed-chain behaviour in simulation. Since URDF does not allow a link to have multiple parents, the mechanism cannot be represented as a true closed chain. To address this limitation, a custom Gazebo system plugin is being developed to enforce the missing constraints at runtime.

The initial objective is to identify the links that would normally be connected in the closed chain and measure the positional error between their attachment points during simulation. Once the attachment points can be tracked reliably, corrective forces can be applied to reduce the error and keep the mechanism assembled.

## Step 4

A custom Gazebo plugin was implemented and attached to the robot model. The plugin locates the required links through the Entity Component Manager (ECM) and retrieves their poses during every simulation step.

Several challenges were encountered during this stage:

* Understanding how links are represented internally as Gazebo entities.
* Retrieving pose information from the ECM.
* Converting attachment point coordinates from local link frames into world coordinates.
* Determining whether forces were being applied at the link origins or at the intended attachment points.
* Debugging plugin loading issues caused by build and URDF configuration errors.

After resolving these issues, the plugin was able to continuously compute the distance between selected attachment points in the mechanism.

## Step 5

Corrective spring-like forces were introduced between the disconnected branches. The force magnitude was calculated from the positional error between corresponding attachment points.

Equal and opposite forces were applied to both sides of the constraint in order to approximate the behaviour of a physical connection while preserving momentum within the system.

During testing, several difficulties were observed:

* Excessively large stiffness values caused the mechanism to become difficult to actuate.
* Incorrect force application points produced unrealistic motion.
* Distinguishing between issues caused by the plugin and issues caused by the robot model required extensive debugging.
* Changes to link masses and inertias resulted in unstable behaviour and temporarily prevented proper controller operation.

Through iterative testing and parameter tuning, stable constraint behaviour was achieved.

## Step 6

The constraint approach was extended to additional branches of the gripper. Multiple attachment point pairs were monitored simultaneously, allowing the mechanism to maintain its overall closed-chain geometry during motion.

The resulting system successfully approximates a closed-chain mechanism while remaining compatible with the tree-based structure required by URDF.

Further work will focus on improving constraint accuracy, analysing simulation stability, and investigating methods for controlling the orientation of the end effector relative to the base link.

here's an application of this concept : https://github.com/KKB007X/closed_chain_gripper
