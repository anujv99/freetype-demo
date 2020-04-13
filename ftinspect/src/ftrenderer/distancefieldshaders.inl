static constexpr char GL_V_DF_SHADER[] = R"(
		#version 450 core

		layout (std140, binding = 0) uniform MVP {
			mat4 projectionView;
		};

		layout (location = 0) in vec2 iPos;
		layout (location = 1) in vec2 iTexCoord;
		layout (location = 2) in int iTexIndex;
		layout (location = 3) in vec4 iColor;

		out vec2 passTexCoord;
		out vec4 passColor;
		flat out int passTexIndex;

		void main() {
			passTexIndex = iTexIndex;
			passTexCoord = iTexCoord;
			passColor = iColor;
			gl_Position = projectionView * vec4(iPos.x, iPos.y, 0.0f, 1.0f);
		}
)";

static constexpr char GL_F_DF_SHADER[] = R"(
		#version 450 core

		layout (binding = 0) uniform sampler2D dfTex;

		out vec4 FragColor;

		uniform float width;
		uniform float edge;

		in vec2 passTexCoord;
		in vec4 passColor;
		flat in int passTexIndex;

		const float smoothness = 1.0f / 16.0f;

		void main() {
		    float distance = 1.0f - texture(dfTex, passTexCoord).r;
			float alpha = 1.0f - smoothstep(width, width + edge, distance);

			FragColor = vec4(1.0f, 1.0f, 1.0f, alpha);
		}
)";