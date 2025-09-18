
# **TreeVigorate**
[![TreeVigorate Demo](https://github.com/user-attachments/assets/4e819922-8ff9-4320-873f-ccbb0ec0432a)](https://vimeo.com/1104978689?fl=pl&fe=vl)

Welcome to TreeVigorate, a procedural parameter driven foliage generator with artist control. We've built this Maya plugin tool for seamless integration into technical art pipelines. Simply hook the plugin into Maya, choose your tree genera, and edit your tree as you watch it grow in accelerated real time. For a quick video demo, just click the image above!

## **Contact**

Eli Asimow : easimow@seas.upenn.edu, https://easimow.com 

Avi Serebrenik : seravi@seas.upenn.edu, https://aviserebrenik.wixsite.com/cvsite

## **Built With**

This tool is a direct implementation of the Siggraph paper *Interactive Invigoration: Volumetric Modeling of Trees with Strands* (2024),  Li, B., Schwarz, N. A., Palubicki, W., Pirk, S., and Benes, B. https://dl.acm.org/doi/10.1145/3658206

To achieve this, we utilized a codebase from the researchers’ prior work in [*Rhizomorph: The Coordinated Function of Shoots and Roots*](https://github.com/edisonlee0212/rhizomorph).

## **Installation**

Installation is straight forward. Simply add this .mll to Maya through the Plugin Manager, and either use the tree description files provided, choose a preset, or write a custom one.
[.mll download](https://github.com/Ser-Avi/TreeVigorate/raw/refs/heads/main/TreeVigorate-8-29-2025.mll) [Tree Descriptions](https://download-directory.github.io/?url=https://github.com/Ser-Avi/TreeVigorate/tree/main/TreeDescriptors)

<img width="512" height="272" alt="image" src="https://github.com/user-attachments/assets/47c89b68-48e7-4085-94aa-8524988af9ad" />


## **Features**

**Strands**

Strands are a systemized representation of the arbor pathways that carry liquid and nutrients from the trunk of a tree to its tips. First modeled in Matthew Holton’s 1994 paper *Strands, Gravity and Botanical Tree Imagery*, they are the basis for our modeling approach here. The idea is that, by simulating those pathways, we can specifically recreate the growth groove patterns found in many common tree trunks. 

We begin this process by initializing with three strand particles at the end nodes of the tree’s branches. Strand particles are then replicated in planar space at preceding tree nodes, merging into larger collections at each branch intersection point. We use standard distance constraint dynamics to adjust particle positions they are transposed down from their progenitors. By the root of the trunk, all strand particles have coalesced into a singular node’s plane.

**Meshing**

We found our source paper’s delauney based meshing method lacking in edge cases. In the branch merging case, for instance, there was no guarantee of strand particle positional consistency. That is, when a joining left branch and right branch would translate all their corresponding particles into their parent node, their positions would become entangled. This meant that bridging the mesh between these three planes risked triangle intersection. 


As one of our priorities was a usable, manifold mesh for artists, this problem proved untenable. Instead, we build Bernstein polynomial splines through the thread of each individual strand particle, and mesh them separately as long tubes. This delivers a manifold mesh while still keeping the grooved look of a strand based tree. The only downside is the finalized mesh’s rather large vertex count. In the Interactive Invigoration authors’ recent publications, they too have transitioned to a new meshing method, and now use delauney triangulation across the entirety of their tree, not just for individual node planes. That’s something we’d love to explore soon, as it seemingly has the best of both our solutions.


**Artist Control**

It was important that our tool be one of active artist participation, rather than a grow and check system. The average artist will have a specific tree in mind; it’s our tool's job to bend its procedural growth to fit that mold as closely as possible. To achieve this, we empower the artist with explicit control over the procedural growth of the tree.

* Sun direction controls the bias of the tree’s directionality. This can be updated ad hoc, shifting the desired growth path of a tree even mid development.

* Invigoration taps directly into the underlying Rhizomorph growth logic, spurring growth from any designated node. Artists can use this like a paint brush, adding length and density where they see fit.

* Pruning recursively removes all children of a selected branch node. This makes for easy selection and removal of lengthy undesirable paths. 

Together, these features make this a tool that controls how, where, and when the tree grows. 



**Parameter based Tree Species**

<img width="464" height="280" alt="image" src="https://github.com/user-attachments/assets/59f1a842-9971-4b44-ba37-56c4fd72bfca" /> <img width="512" height="360" alt="image" src="https://github.com/user-attachments/assets/77940044-a5df-4ca5-a2d4-c4fa8f974c48" />



**Intuitive Maya GUI**

<img width="512" height="368" alt="image" src="https://github.com/user-attachments/assets/a61866c8-16f3-40d8-98d5-3a5bbe95e06f" />

**Leaf Instancer Support**

Once you are happy with your tree’s structure, save the leaf positions with the Export Leaf Matrices option. This command also writes these matrices to treenode.leafLocations for convenience. 
Use the Maya instancer node to generate leaves at the saved leaf positions.

Example MEL: instancer; connectAttr Leaves.matrix instancer1.inputHierarchy[0]; connectAttr TN1.leafLocations instancer1.inputPoints;

Tip: consider instancing a group with your leaf mesh as a child object, rather than instancing the leaf mesh itself. This allows for easy repositioning of the leaf pivot point.

