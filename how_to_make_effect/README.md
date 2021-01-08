# How to make an transition effect

## Definition of an transition effect
A transition effect can be described by an `xml` config file and some `alpha` images, the `xml` file descripted several informations, like: the effect name, duration, front side, alpha number and the most important part, the perspective matrix in key time stamp.

In one word, there are two part for a transition effect, alpha channel and perspective matrix, some effect could be archived by one of them and others need them work together.

Alpha channels, defines the transparency between two images during the transition, it could be a single value for whole image for a moment, or a row/column based vector which means the transparency of one dimension consistant, or even a matrix to define the transparency on each pixels.

Perspective matrix, defines the moving path and shape of the images, for example an matrix `[1,0,0;0,1,0;0,0,1]` to `[1.2,0,-1.2;0,1.2,-1.2;0,0,1]` defines an animation of zoom out and move left, and of course you can define a matrix of rotation. Some `matlab` code are supported as auxilary to do perspective calculation due to the implicit understand on matrix for human.


Here's an example of `EraseOut`:
```
<effect name="EraseOut" alphanum="31" duration="1.5" front="A" background="0">
    <script pos="0">
        <A>1,0,0,0,1,0,0,0,1</A>
        <B>1,0,0,0,1,0,0,0,1</B>
        <A43>1,0,0,0,1,0,0,0,1</A43>
        <B43>1,0,0,0,1,0,0,0,1</B43>
        <A169>1,0,0,0,1,0,0,0,1</A169>
        <B169>1,0,0,0,1,0,0,0,1</B169>
    </script>
    <script pos="1">
        <A>1,0,0,0,1,0,0,0,1</A>
        <B>1,0,0,0,1,0,0,0,1</B>
        <A43>1,0,0,0,1,0,0,0,1</A43>
        <B43>1,0,0,0,1,0,0,0,1</B43>
        <A169>1,0,0,0,1,0,0,0,1</A169>
        <B169>1,0,0,0,1,0,0,0,1</B169>
    </script>
</effect>
```

## Guidance
There are lots of tools could be used during making a effect, like photoshop, matlab, R. This project support several matlab functions to visualize the transition.

`im2video` also provide an command `--predict ../effects/diagonal1/pts_a_1.txt` to calculate perspective matrix, this be used when the point coordinate of the animation is known, for example:
we got the point transformation in file `pts_a_1.txt` in format `[x y x' y']`, `[x,y]` denotes the start coordinate and `[x',y']` denotes the end coordinate, for an image, we need at least three point to demonstrate the gesture in 3-d dimension world.
```
0,0,0,0.99
1,0,0.01,0.99
1,1,0.01,1
0,1,0,1
```

The perspective matrix will looks like:
```
<INFO> Find 4 points
<INFO> Perspective Transform matrix maybe:
0.01000 0.00000 -0.00000 0.00000 0.01000 0.99000 0.00000 0.00000 1.00000
<INFO> Affine Transform matrix maybe:
0.01000 0.00000 0.00000 0.00000 0.01000 0.99000 0.00000 0.00000 1.00000
```
