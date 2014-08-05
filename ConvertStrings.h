#ifndef CONVERTSTRINGS_H_INCLUDED
#define CONVERTSTRINGS_H_INCLUDED

#include <Wt/WLocalizedStrings>

class ConvertStrings : public Wt::WLocalizedStrings
{
protected:
    virtual bool resolveKey( const std::string& key, std::string& result )
    override
    {
        if( key.empty() )
            return true;

        std::vector< wchar_t > w;
        w.resize( key.size() * sizeof( wchar_t ) * 2 );
        MultiByteToWideChar( CP_ACP, 0, key.c_str(), -1, &w[ 0 ], w.size()
                           );

        vector< char > s;
        s.resize( w.size() );
        WideCharToMultiByte( CP_UTF8, 0, &w[ 0 ], -1, &s[ 0 ], s.size(), 0,
        0 );

        result = &s[ 0 ];
        return true;
    }
};

#endif // CONVERTSTRINGS_H_INCLUDED
