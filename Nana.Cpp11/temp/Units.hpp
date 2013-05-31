#ifndef NANA_GUI_Units_HPP
#define NANA_GUI_Units_HPP

#include <iostream> 
#include <map>
#include <set>
#include <functional>
#include <sstream>


class CUnit;
 std::ostream& operator<<( std::ostream& o, const CUnit& u);

class CUnit
{
  public:   
    typedef std::string                             unit_name       ;
    typedef std::string                             magnitude_name  ;
    typedef std::set<unit_name>                     magnitude_t     ;
    typedef std::map<unit_name     , CUnit      >   units           ;
    typedef std::map<magnitude_name, magnitude_t>   magnitudes      ;
    typedef std::function <double(double)>          nonLinealFunction;
    struct conversion
    {
        double              c, s;
        bool                lineal;
        nonLinealFunction   nlc;
        conversion()   :c(1),s(0)  , lineal(true)
        {};
        conversion(double c_  , double s_=0)            :c(c_),s(s_), nlc(_identity), lineal(true)
        {};
        conversion(double c_  , double s_ ,const nonLinealFunction& nlc_):c(c_),s(s_), nlc(nlc_)    , lineal(false) 
        {};
        conversion operator*(const conversion& rc)
        { 
            if (lineal && rc.lineal)
                return conversion (rc.c*c, c*rc.s+s);
            if (lineal )
                return conversion (rc.c*c, c*rc.s+s, rc.nlc);
            if (rc.lineal )
                return conversion (c,s, [rc,this](double b){return nlc(rc.c*b+rc.s);});

            return conversion (c,s, [rc,this](double b){return nlc(rc.c*rc.nlc(b)+rc.s);});
        }
    } ;
    conversion      conv;
    unit_name       name, base;
    magnitude_name  magnitude;
    bool            error;
    static const magnitudes& MagnitudesDic() {return _Magnitudes;}
    static const units     & UnitsDic     () {return _Units     ;}
    static bool       unit_exist(const unit_name& n)
    {  
        return _Units.find(n)!= _Units.end();  
    }
    static bool       magnitude_exist(const unit_name& m)
    {  
        return _Magnitudes.find(m)!= _Magnitudes.end();  
    }
    std::string       to_string()
    {
        std::ostringstream o;
        o << *this;
        return o.str();
    }
  private:
    static units                _Units ;
    static magnitudes           _Magnitudes;
    static nonLinealFunction    _identity;

    void add();

  public:
    CUnit(const unit_name& name_,  double   k_            ,   const unit_name& base_,   magnitude_name magnitude_="")
        : name(name_), base(base_), magnitude(magnitude_), error(true), conv(k_)
    {
        add();
    }
    CUnit(const unit_name& name_,  const conversion& conv_,   const unit_name& base_,   magnitude_name magnitude_="")
        : name(name_), base(base_), magnitude(magnitude_), error(true), conv(conv_)
    {
        add();
    }
    CUnit():error(true){}
    CUnit(const unit_name& name_, const unit_name& base_)
        : name(name_), base(base_), error(true)
    {
        if (! (unit_exist(base) && unit_exist(name)) )
        {    
           std::cerr << std::endl << "We need two existing units in order to create a new definition of: " << name
                     << " based on " << base << std::endl;
                return;
         }
        if (_Units[name].magnitude != _Units[base].magnitude)
        {   
            std::cerr << std::endl << "Units "<< name<< " of " << _Units[name].magnitude << " and "
                     << base << " of " << _Units[name].magnitude<<" are not compatible " <<  std::endl;
                return;
        }
        if (name==base)
        {    
            error=false;
            return ;
        }
        if (_Units[name].base == base)
        {    
            *this= _Units[name];
            error=false;
            return ;
        }

 /*       if (_Units[name].base== name)
        CUnit u();*/
    }


};



#endif 

//typedef std::map<std::string, Magnitude> Magnitudes;
//extern Magnitudes AllUnits ;
//extern Magnitude MMasa ;
//extern Magnitude MTemperature ;
//extern Magnitude MEnergy ;
//extern Magnitude MLength ;
//extern Magnitude MTime ;
//extern Magnitudes AllUnits ;
//typedef std::multimap<std::string, 
//extern std::multimap<std::string, 
    //typedef std::map<std::string,std::pair<double,std::string>> magnitude;


