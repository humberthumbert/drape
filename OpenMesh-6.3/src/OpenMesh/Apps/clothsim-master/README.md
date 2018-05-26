**Software and Hardware**
=========================  
  
We used a UTCS lab machine running Ubuntu 14.04 (kernel version
3.13.0-83-generic) with 16GB of RAM, an Intel Xeon(R) CPU E3-1270 V2 @ 3.50GHz
processor with 8 cores, and an NVIDIA Quadro 600/PCIe/SSE2 graphics card. Our
project was implemented from scratch using OpenGL and GLM for the simulation
and GLFW for the GUI. To build our project, run `make` from the top of the
project hierarchy. To build and run, use `make run`.

**Overview**
============  

Our primary goal was to simulate visually realistic cloth in real-time and have
the cloth collide with rigid-bodies made of triangle meshes. One stretch goal
we were able to hack on (but not complete) was self-collision detection.  
  
**Design and Implementation**
=============================  

Model
=====  

We use a mass-spring model, meaning our cloth consists of a grid of evenly
spaced nodes, connected by springs. Each node has a position, velocity, and
mass. We use three types of springs: structural, shear, and flexion. The
structural springs connect a node at (i, j) with its neighbors at (i-1, j),
(i+1, j), (i, j-1), and (i, j+1). These springs are constrained when the cloth
is stretched. The shear springs connect a node at (i, j) with its neighbors at
(i-1, j-1), (i+1, j-1), (i-1, j+1), and (i+1, j+1). These springs are
constrained when the cloth is under shear stress. Finally, the flexion springs
connect a node at (i, j) with its neighbors at (i-2, j), (i+2, j), (i, j-2),
and (i, j+2). These springs are constrained when the cloth bends.

With just our basic mass-spring model, we found that our cloth was too elastic
and would slowly stretch in the direction of gravity (to infinity). Damping was
insufficient to correct this problem. Also, our model was still prone to
explosions. To correct this, we use the method described in [1]. After
integration in every frame, we check each structural spring to see if its
length has reached a threshold, called the critical deformation rate, and if
so, we “correct” the spring by adjusting the positions and velocities of the
corresponding nodes to bring the length of the spring back under the critical
deformation rate. We found a critical deformation rate of 0.05 (meaning that
springs are corrected if they exceed their rest length by 5%) gave visually
pleasing results. For reference, the example in [1] uses a critical deformation
rate of 0.1.

Our cloth is animated using velocity Verlet integration. We tried using
semi-implicit Euler and position Verlet methods, but the velocity Verlet proved
to be the most stable integrator out of the three. Use of different integrators
gave similar results, but affected the time at which the cloth would start
gaining energy and explode.

Finally, we draw our cloth by joining directly adjacent nodes into triangles.
We shade all of the objects in our scene using a simple lighting model (diffuse
shading with a single fixed light source) for realism. For the cloth,
calculation of the vertex normals is done in the vertex shader by averaging the
normals of the triangles defined by adjacent vertices.

Collisions
==========  

To detect point-triangle collisions, we use a method similar to the ones
described in [2] and [3]. In short, we compute the barycentric coordinates of
the point in question and check if the point falls in the triangle. By using
ray semantics, we are able to deduce a reasonable response for the point on the
cloth.

We use a simple spatial hash data structure to efficiently detect collisions
between nodes of the cloth. The data structure relies on a custom hash function
and groups nodes into buckets by spatial locality. This data structure is
constructed during each frame and is used by nodes to quickly find candidates
for collisions. However, there are several immediate flaws with our method,
most noticeably that constructing the data structure each frame is unnecessary,
our hash is quite crude, and we do not consider velocity at all. Despite all
this, we found that using the spatial hash made checking for self-collisions a
negligible overhead (the naive O(n<sup>2</sup>) solution introduced a slight
lag into the simulation).

We use two types of collision response. The first is the penalty method. Under
our implementation of the penalty method, collision response begins after a
collision has happened. The response is a repulsive force to drive the
colliding object “away” from the collision (e.g. to push a cloth out of a ball
it has penetrated). This force is in the direction of the shortest path out of
the collision target and increases the more the object goes into the target. To
prevent too much penetration, we compute collision detection using an offset
from the actual surface of the object so that the penalty response begins
preemptively. For instance, in our collisions with a sphere object, we scale
the radius by 1.1 before doing detection. Note that the penalty method does not
prevent collisions but instead attempts to reverse collisions when they occur.

The second type of collision response we use is an impulse method. Unlike the
penalty method, our impulse method attempts to prevent collisions altogether.
To do this, we check for collisions in the next frame and apply an
instantaneous correction to the trajectory of objects that are going to
collide. This correction involves adjusting the velocity of objects within the
frame to simulate the onset of an impulse. In resting collisions, we use
position correction to prevent bouncing and fall through. We found that an
impulse response alone is typically not sufficient to achieve realistic
collisions. Instead, we combine impulse responses with the penalty method to
reduce penetration and fall through artifacts.

**User Interface**
==================  

Our GUI is minimal and is generated using GLFW. We support a few keyboard
shortcuts for controlling the simulation:

| Key Combo  |    |   Description                                |
| :----------|----|:---------------------------------------------|
| SPACE      |    |   start and stop                             |
| N          |    |   step one frame                             |
| R          |    |   restart                                    |
| WASDQE     |    |   move camera                                |
| SHIFT-WASD |    |   rotate camera around the look-at           |
| F          |    |   toggle fill                                |
| L          |    |   toggle wireframe                           |
| P          |    |   toggle points                              |
| ESC        |    |   exit                                       |

<br>

**Results**
===========  

These videos do not accurately reflect the quality of the simulations that can be gotten in the labs in GDC (we don\'t have the necessary privileges for installing screen recording software on those machines).  

Since GitHub flavored markdown doesn't allow embedded videos, [here's](https://www.youtube.com/playlist?list=PLV4RyI54A89PP-EG9UFeHOohGK-WjRQCt) a link to our YouTube playlist.  

**Future Work**
===============  

There are a number of improvements we want to implement but didn’t have time
for. First, a better space-partitioning data structure is necessary to do more
effective self-collision detection. We explored several external libraries for
such a data structure but most options were too heavyweight for our purposes or
difficult to install with our limited privileges on the lab machines. Aside
from using a better partitioning structure, incorporating thickness into
collision detection and response would likely yield much better results (fewer
penetration artifacts).

Finally, several improvements to the user interface can be added. For instance,
we would like to allow the user to manipulate objects in the scene using the
mouse. We would also like to add buttons to the GUI for controlling the
simulation.

**Conclusion**
==============  

Simulating cloth is hard.

**References**
==============  

[1] PROVOT, X. 1995. Deformation Constraints in a Mass-Spring Model to Describe Rigid Cloth Behavior.  

[2] PROVOT, X. 1997. Collision and Self-Collision Handling in Cloth Model Dedicated to Design Garments.  

[3] ERICSON, C. 2004. Real-Time Collision Detection.  

[4] BRIDSON, R, FEDKIW, R & ANDERSON, J. 2002. Robust Treatment of Collisions, Contact and Friction for Cloth Animation.  

[5] [https://open.gl](https://open.gl)  

[6] [Dr. Vouga](http://www.cs.utexas.edu/users/evouga/)  
