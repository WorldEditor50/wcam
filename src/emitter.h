#ifndef EMITTER_H
#define EMITTER_H

#include <QObject>
#include <memory>

class Emitter : public QObject
{
    Q_OBJECT
public:
    explicit Emitter(QObject *parent = nullptr);

signals:
    void terminate();
public:
    inline static Emitter* ptr()
    {
        static std::unique_ptr<Emitter> emitter(new Emitter);
        return emitter.get();
    }
};

#endif // EMITTER_H
