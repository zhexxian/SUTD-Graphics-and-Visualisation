//weather or not to render in real time
var RealTime:boolean = false;

//How much of our screen resolution we render at
var RenderResolution:float = 1;

private var renderTexture:Texture2D;
private var lights:Light[];

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
    //Gather all lights
    lights = FindSceneObjectsOfType(typeof(Light)) as Light[];

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
    //The color we change throught the function
    var returnColor:Color = Color.black;

    var hit:RaycastHit;

    if (Physics.Raycast(ray, hit)) {

        //The material of the object we hit
        var mat:Material;

        //Set the used material
        mat = hit.collider.GetComponent.<Renderer>().material;

        //if the material has a texture
        if (mat.mainTexture) {
            //return the color of the pixel at the pixel coordinate of the hit
            returnColor += (mat.mainTexture as Texture2D).GetPixelBilinear(hit.textureCoord.x, hit.textureCoord.y);
        }
        else {
            //return the material color
            returnColor += mat.color;
        }

        returnColor *= TraceLight(hit.point + hit.normal*0.0001);
    }

    //The color of this pixel
    return returnColor;
}

//Trace a single point for all lights
function TraceLight(pos:Vector3):Color {
    //Set starting light to that of the render settings
    var returnColor:Color = RenderSettings.ambientLight;

    //We loop through all the lights and perform a light addition with each
    for (var light:Light in lights) {
        if (light.enabled) {
            //Add the light that this light source casts to the color of this point
            returnColor += LightTrace(light, pos);
        }
    }
    return returnColor;
}

//Trace a single point for a single light
function LightTrace(light:Light, pos:Vector3):Color {
    //Trace the directional light
    if (light.type == LightType.Directional) {
        if (Physics.Raycast(pos, -light.transform.forward)) {
            return Color.black;
        }
        return light.color*light.intensity;
    }
}