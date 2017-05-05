//weather or not to render in real time
var RealTime:boolean = false;

//Vary from [0,1], to reduce the render texture resolution for faster ray tracing
var RenderResolution:float = 1;

private var renderTexture:Texture2D;

//Create render texture with screen size with resolution
function Awake() {
    renderTexture = new Texture2D(Screen.width*RenderResolution, Screen.height*RenderResolution);
}

//Do one raytrace when we start playing
function Start() {
    if (!RealTime) {
        RayTrace();
    }
}

//Real Time Rendering
function Update() {
    if (RealTime) {
        RayTrace();
    }
}

//Draw the render
function OnGUI() {
    GUI.DrawTexture(Rect(0, 0, Screen.width, Screen.height), renderTexture);
}

//The function that renders the entire scene to a texture
function RayTrace():void {
    for (var x:int = 0; x < renderTexture.width; x += 1) {
        for (var y:int = 0; y < renderTexture.height; y += 1) {

            //Now that we have an x/y value for each pixel, we need to make that into a 3d ray
            //according to the camera we are attached to
            var ray:Ray = GetComponent.<Camera>().ScreenPointToRay(Vector3(x/RenderResolution, y/RenderResolution, 0));

            //Now lets call a function with this ray and apply it's return value to the pixel we are on
            //We will define this function afterwards
            renderTexture.SetPixel(x, y, TraceRay(ray));
        }
    }

    renderTexture.Apply();
}

//Trace a Ray for a singple point
function TraceRay(ray:Ray):Color {

    if (Physics.Raycast(ray)) {
        return Color.white;
    }

    return Color.black;
}