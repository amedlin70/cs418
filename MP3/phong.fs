varying vec3 n;
varying vec3 v;

uniform sampler2D myTexture1;
uniform sampler2D myTexture2;

void main (void)
{
vec3 l = normalize(gl_LightSource[0].position.xyz - v);
vec3 e = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
vec3 nhat = normalize(n);
vec3 r = normalize(-reflect(l,nhat));
vec4 texval1 = texture2D(myTexture1, vec2(gl_TexCoord[0]));
vec4 texval2 = texture2D(myTexture2, vec2(gl_TexCoord[1]));

//calculate Ambient Term:
vec4 Iamb = gl_FrontLightProduct[0].ambient;

//calculate Diffuse Term:
vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(nhat,l), 0.0);
Idiff.a = 1.0;

// calculate Specular Term:
vec4 Ispec = gl_FrontLightProduct[0].specular
                  * pow(max(dot(r,e),0.0),0.3*gl_FrontMaterial.shininess);
Ispec.a = 1.0;

// write Total Color:
//gl_FragColor = (Iamb + Idiff + Ispec) * .5 * (texval1 + texval2);
gl_FragColor = (Iamb + Idiff + Ispec) * texval1;
//gl_FragColor = texval1;

}
