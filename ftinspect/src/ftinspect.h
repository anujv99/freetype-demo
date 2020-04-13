
#ifndef __FT_INSPECT_H__
#define __FT_INSPECT_H__

#include <core/layer.h>

#include "ftgrid.h"

class FTInspect : public engine::core::Layer {
public:
	FTInspect();
	~FTInspect();
private:
	void OnImGuiUpdate() override;
	void OnRender() override;
	void OnUpdate(engine::core::TimePoint dt) override;

	void SetupImGui();
private:
	FTGrid m_FTGrid;

	int gi = 0, pt = 16;
};

#endif //__FT_INSPECT_H__
