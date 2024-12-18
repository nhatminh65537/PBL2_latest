#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H
#include <string>


class Identifiable
{
    public:
        explicit Identifiable(const std::string& = "null");
        virtual ~Identifiable();
        [[nodiscard]]  const std::string& GetID() const;
        void SetID (const std::string&);
        virtual void Show() const = 0;
    protected:
        std::string ID;
};

#endif // IDENTIFIABLE_H
