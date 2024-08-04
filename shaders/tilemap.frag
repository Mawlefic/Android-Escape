#version 100
precision mediump float;

uniform sampler2D tileset;
uniform sampler2D tilemap;
uniform float width;
uniform float height;
uniform vec2 screenSize;
uniform vec2 offset;

void main()
{
    float tileX = (gl_FragCoord.x + offset.x) / width;
    float tileY = mod(gl_FragCoord.y + offset.y, height) / height;
    vec4 tile = texture2D(tilemap, vec2(tileX, tileY));
    float tileNum = tile.r * 15.0; // 0 - 15
    vec2 tileID = vec2(mod(tileNum, 4.0) / 4.0, floor(tileNum / 4.0) / 4.0);
    float finalX = tileID.x;
    float finalY = tileID.y;
    float a = mod((gl_FragCoord.x + offset.x) / 2.0, 20.0) * .01;
    float b = mod((gl_FragCoord.y + offset.y) / 2.0, 20.0) * .01;
    vec4 texelColor = texture2D(tileset, vec2(finalX + (a * 1.25), finalY + (b * 1.25)));
    gl_FragColor = vec4(texelColor.rgb, 1.0);
}