
#version 310 es

precision mediump float;

// OUTPUT
layout(location = 0) out vec4 outNormalDepth;
layout(location = 1) out vec4 outDiffuseRoughness;
layout(location = 2) out vec4 outEmission;

uniform vec4 uColor;

void main()
{
	outEmission.rgb = normalize(uColor.rgb);
	outEmission.a = uColor.a; // light id
}
