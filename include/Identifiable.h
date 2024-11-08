#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H
#include <string>

using namespace std;

class Identifiable
{
    public:
        explicit Identifiable(const string& = "null");
        virtual ~Identifiable();
        [[nodiscard]]  const string& GetID() const;
        void SetID (const string&) ;
        virtual void Show() const = 0;
    protected:
        string ID;
};

#endif // IDENTIFIABLE_H
