static constexpr char GL_V_FONT_SHADER[] = R"(
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

static constexpr char GL_F_FONT_SHADER[] = R"(
		#version 450 core

		layout (binding = 0) uniform sampler2D fontTex;

		out vec4 FragColor;

		uniform float gamma;

		in vec2 passTexCoord;
		in vec4 passColor;
		flat in int passTexIndex;

		void main() {
			if (passTexIndex < 0) {
				FragColor = passColor;
				return;
			}
			
			vec3 pixelColor = vec3(texture(fontTex, passTexCoord).r);

			pixelColor = pow(pixelColor, vec3(1.0f / gamma));

			FragColor = vec4(vec3(pixelColor), 1.0f);
		}
)";