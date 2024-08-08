#ifndef SEARCH_HPP
#define SEARCH_HPP

class Search {
protected:
    static bool on;

public:
    [[nodiscard]] static bool getOn() ;

    static void setOn(bool value);
};

#endif
