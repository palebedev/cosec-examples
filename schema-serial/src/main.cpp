#include <widget_generated.h>

#include <ce/hex.hpp>

#include <iostream>

int main()
{
    flatbuffers::FlatBufferBuilder builder{256};
    auto name = builder.CreateString("zerg");
    widgets::point2d pos{-3,5};
    double opacity = 1.0;
    uint64_t frob_data[] = {12,34,567};
    auto frobs = builder.CreateVector(frob_data,sizeof frob_data/sizeof *frob_data);
    auto w = Createwidget(builder,name,&pos,opacity,frobs);
    builder.Finish(w);
    std::span<const std::byte> data{reinterpret_cast<const std::byte*>(builder.GetBufferPointer()),
                                    builder.GetSize()};
    std::cout << ce::hex(data) << '\n';

    flatbuffers::Verifier v{reinterpret_cast<const std::uint8_t*>(data.data()),data.size()};
    if(widgets::VerifywidgetBuffer(v)){
        auto w2 = widgets::Getwidget(data.data());
        std::cout << w2->name()->c_str() << " : (" << w2->pos()->x() << ',' << w2->pos()->y() << ")\n";
    }else
        std::cerr << "Invalid data!\n";
}
