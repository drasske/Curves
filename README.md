# Curves
Implementation of curves, polynomial interpolation, and splines.
For all projects points can be moved using the mouse and reset using the reset button.
For projects 2, 3, and 4 points can be created by clicking in the window.

### Project 1 - De Casteljau Algorithm for Polynomial Functions
Illustrates how the coefficients of Bernstein polynomials affect the shape of a polynomial graph. The interface has a button which changes the degree of the polynomial. The default graph has all coefficients equal to 1, which simply gives output 1 for all _t_. The values of the polynomial can be calculated using either Nested Linear Interpolation (NLI) or computing the BB-Form of the polynomial.

![Project1Image](https://user-images.githubusercontent.com/35553333/80566878-cc1a2d00-89a8-11ea-8d23-992140824956.png)

### Project 2 - De Casteljau Algorithm for Bezier Curves
Displays a Bezier Curve based on the control points. The interface allows for toggling the visibility of the control polyline as well as the shells for NLI. Three methods are given, NLI-Form, BB-Form, and Midpoint Subdivision.

![Project2Image](https://user-images.githubusercontent.com/35553333/80567154-6b3f2480-89a9-11ea-9278-d61f652b41cb.png)

### Project 3 - Interpolating Polynomials
Creates a parametric polynomial curve passing through all input points. The Newton form of the polynomial is used for each coordinate _x(t)_, _y(t)_. The curve is guaranteed to pass through all points but may behave erratically.

![Project3Image](https://user-images.githubusercontent.com/35553333/80568667-4b5d3000-89ac-11ea-9151-62d65564e63f.png)

### Project 4 - Interpolating Splines
Displays an interpolated spline with _C2_ continuity. The spline function is written as a series of truncated power functions. Solving for the splines _x(t)_ and _y(t)_ involves solving two _(n+2)X(n+2)_ linear systems.

![Project4Image](https://user-images.githubusercontent.com/35553333/80568748-6cbe1c00-89ac-11ea-82da-6c8e338563d3.png)

