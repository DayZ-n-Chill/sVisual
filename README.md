# sVisual

<p align="center">
		<img src="https://img.shields.io/github/v/tag/simonvic/sVisual?color=F0544C&label=latest%20stable%20release&style=for-the-badge">
        <br>
		<img src="https://img.shields.io/github/license/simonvic/sVisual?color=F0544C&style=flat-square">
</p>

# Getting started

sVisual is a mod for DayZ. It aims to improve the PostProcessing effects and make them easier to work with, while allowing for extensibility and easy tweaking. 
It also adds some new features like:
- Dynamic Depth of Field
- Headbob
- Motion blur
- Camera improvements

The core of this mod is the `PPEManager`, which takes care of all PostProcess Effects, defined using a `PPEParams`

# The insides


<h3 align="center">PostProcess Effect Manager</h3>
This is a small diagram roughly showing how the PPEManager works
<img src="https://imgur.com/y7Dw4LL.png">


---
<br>

<h3 align="center">PostProcess Effect Params</h3>
This is a rough (pretty bad) UML diagram of the most important classes, to help you understand how animations and PPEParams are structured.
<img src="https://imgur.com/zamIqli.png">

---
<br>

## PPEParams
`PPEParams` is the "container" of any PostProcess Effect you wish to add to it (e.g. saturation, vignette, motion blur etc.).
```csharp
ref PPEParams myPPE = new PPEParams();
```
To add a parameter use the `setParam` method...
```csharp
// Set vignette effect intensity and color
myPPE.setParam(MaterialNames.GLOW, PPEParamNames.VIGNETTE_STRENGTH, 0.69 );
myPPE.setParam(MaterialNames.GLOW, PPEParamNames.VIGNETTE_COLOR, PPEManager.getPPEColor(R,G,B,A));
```
or more easly use the provided setters:
```csharp
myPPE.setVignette(intensity, color);
myPPE.setRadialBlur(powerX, powerY, offsetX, offsetY);
myPPE.setChromAber(powerX, powerY);
//...
```

To apply it, "hand it over" to the PPEManager, which will calculate the correct value of all active PPEParam and then apply it
```csharp
PPEManager.activate(myPPE);
```
and to deactivate it:
```csharp
PPEManager.deactivate(myPPE);
```

<br>

## PPEAnimatedParams
A `PPEAnimatedParams` is just like a `PPEParams`, but it has an animation mechanism (entirely managed by the `PPEManager`), which allows you to animate the values of a PostProcess effect.

A `PPEAnimatedParams` is an *abstract* class. You need to create your own class and override the `onAnimate()` method, which will be called on every frame.
The currently are two types of Animated Params: 
- `PPELoopedParams` It will play until stopped or deactivated
- `PPETimedParams` It has timing mechanism that will stop/deactivate it when it ends

To create your animation, simply extend either `PPELoopedParams` or `PPETimedParams`
```csharp
class MyLoopAnimation : PPELoopedParams{
	override void onAnimate(float deltaTime){
		/* change PPE values here
		setOverlay(...);
		setChromAber(...);
		setCameraEffects(...);
		*/
		setVignetteIntensity( Math.Sin(getTime()) );
	}
}

class MyTimedAnimation : PPETimedParams{
	override void onAnimate(float deltaTime){
		setVignetteIntensity( Math.Cos(getTime()) );
	}
}
```

A `PPETimedAnimation` also has a "duration" which can be set with the constructor, or the provided method:
```csharp
ref MyTimedAnimation myTimedAnimation = new MyTimedAnimation(6); // the animation will last 6 seconds
myTimedAnimation.setDuration(10.0); // the animation will last 10 seconds
```

To activate an animation simply hand it over to the `PPEManager`
```csharp
ref MyLoopAnimation myAnimation = new MyLoopAnimation();
ref MyTimedAnimation myTimedAnimation = new MyTimedAnimation(5.5);
PPEManager.activate(myAnimation);
PPEManager.activate(myTimedAnimation);
```
If you want to manually manage the animation you can use the provided methods
```csharp
myAnimation.start(); // Set the animation state to "Playing"
myAnimation.stop(); // Reset all values to default and set the animation state to "Stopped"
myAnimation.pause(); // Freeze the animation values and set the animation state to "Paused"
myAnimation.resume(); // Resume the the animation and set the animation state to "Playing"
```

<br>
<br>

# Contact me
<h2 align="center">Found a bug or want to give a suggestion? Feel free to contact me!</h2>
<p align="center">
    <a style="margin: 0 10px" href = "mailto: simonvic.dev@gmail.com">
        <img src="https://img.shields.io/badge/Email-simonvic.dev%40gmail.com-F0544C?style=social&logo=data:image/png;base64,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&logoColor=F0544C">
    </a>
    <br>
    <a style="margin: 0 10px" href="https://matrix.to/#/@simonvic:matrix.org">
        <img src="https://img.shields.io/badge/Matrix-%40simonvic%3Amatrix%3Aorg-F0544C?logo=element&style=social">
    </a>
    <br>
    <img style="margin: 0 10px" src="https://img.shields.io/badge/Discord-simonvic%239804-F0544C?logo=Discord&style=social">
    <br>
    <a style="margin: 0 10px" href="https://twitter.com/_simonvic_">
        <img src="https://img.shields.io/badge/Twitter-%40simonvic-F0544C?logo=twitter&style=social">
    </a>
</p>

---

<h3 align="center" >Buy me a coffee :)</h3>
<p align="center">
    <img width="40px" src="https://i.imgur.com/e3kk9J4.png">
    <a href = "https://paypal.me/simonvic">
        <img src="https://img.shields.io/badge/Paypal-donate-F0544C?style=for-the-badge&logo=paypal&logoColor=F0544C">
    </a>
</p>