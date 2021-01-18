#ifndef READER_H
#define READER_H

#include <escher.h>

namespace reader{

    class App : public ::App{
        public:
            class Descriptor : public ::App::Descriptor{
                public:
                    I18n::Message name() override;
                    I18n::Message upperName() override;
                    const Image * icon() override;
            };
            class Snapshot : public ::App::Snapshot{
                public:
                    App * unpack(Container * container) override;
                    Descriptor * descriptor() override;
            };
        private:
            App(Snapshot * snapshot);
    };
}
#endif