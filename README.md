
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

Installation is straight forward. Simply connect the .mll plugin within your Maya environment.

<img width="512" height="272" alt="image" src="https://github.com/user-attachments/assets/47c89b68-48e7-4085-94aa-8524988af9ad" />

## **Features**

Parameter based Tree Species

<img width="464" height="280" alt="image" src="https://github.com/user-attachments/assets/59f1a842-9971-4b44-ba37-56c4fd72bfca" /> <img width="512" height="360" alt="image" src="https://github.com/user-attachments/assets/77940044-a5df-4ca5-a2d4-c4fa8f974c48" />



Intuitive Maya GUI 

<img width="512" height="368" alt="image" src="https://github.com/user-attachments/assets/a61866c8-16f3-40d8-98d5-3a5bbe95e06f" />

Leaf Instancer Support

Once you are happy with your tree’s structure, save the leaf positions with the Export Leaf Matrices option. This command also writes these matrices to treenode.leafLocations for convenience. 
Use the Maya instancer node to generate leaves at the saved leaf positions.

Example MEL: instancer; connectAttr Leaves.matrix instancer1.inputHierarchy[0]; connectAttr TN1.leafLocations instancer1.inputPoints;

Tip: consider instancing a group with your leaf mesh as a child object, rather than instancing the leaf mesh itself. This allows for easy repositioning of the leaf pivot point.

