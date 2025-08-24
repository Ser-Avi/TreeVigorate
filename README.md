
TreeVigorate

Welcome to TreeVigorate, a procedural parameter driven foliage generator. We've built this Maya plugin tool for seamless integration into technical art pipelines. Simply hook the plugin into Maya, choose your tree genera, and watch the tree grow in accelerated real time.  

Contact

Eli Asimow : easimow@seas.upenn.edu, easimow.com 

Avi Serebrenik : seravi@seas.upenn.edu, https://aviserebrenik.wixsite.com/cvsite
Built With

This tool is a direct implementation of the Siggraph paper:

Interactive Invigoration: Volumetric Modeling of Trees with Strands (2024),  Li, B., Schwarz, N. A., Palubicki, W., Pirk, S., and Benes, B.

To achieve this, we utilized a codebase from the researchers’ prior work in Rhizomorph: The Coordinated Function of Shoots and Roots. 
Installation
installation is straight forward. Simply connect the .mll plugin within your Maya environment.



Features

Parameter based Tree Species



Intuitive Maya GUI 


Leaf Instancer Support

Once you are happy with your tree’s structure, save the leaf positions with the Export Leaf Matrices option. This command also writes these matrices to treenode.leafLocations for convenience. 
Use the Maya instancer node to generate leaves at the saved leaf positions.

Example MEL: instancer; connectAttr Leaves.matrix instancer1.inputHierarchy[0]; connectAttr TN1.leafLocations instancer1.inputPoints;

Tip: consider instancing a group with your leaf mesh as a child object, rather than instancing the leaf mesh itself. This allows for easy repositioning of the leaf pivot point.

