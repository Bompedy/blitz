#include "blitz/provider.h"

int main() {

    auto provider = blitz::Provider(1, 1);

    provider.close();
    return 0;
}
