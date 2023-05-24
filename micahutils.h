#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

namespace micah
{
    struct MessageOnDestructor
    {
        MessageOnDestructor( std::string const& message ) :
            m_message( message ) {}
        virtual ~MessageOnDestructor()
        {
            std::cerr << m_message << '\n';
        }

        std::string m_message;
    };

    struct Timer
    {
        Timer() = default;
        explicit Timer( std::string name ) :
            m_name( name ) {}

        virtual ~Timer() noexcept
        {
            if( !m_durationChecked )
            {
                check_duration();
            }
        }

        void reset() noexcept
        {
            m_start = std::chrono::steady_clock::now();
        }

        void check_duration() noexcept
        {
            m_durationChecked = true;

            if( auto const duration_ms{ std::chrono::duration_cast< std::chrono::duration< double, std::milli > >( std::chrono::steady_clock::now() - m_start ).count() }; duration_ms > 1.0 )
            {
                std::cerr << "Time spent" << ( m_name.empty() ? "" : " (" + m_name + ")" ) << ": " << duration_ms << "ms\n";
            }
        }

        std::string m_name;
        std::chrono::steady_clock::time_point m_start{ std::chrono::steady_clock::now() };
        bool m_durationChecked{};
    };

    struct CumulativeTimer
    {
        double sum_us_{};
        int count_{};
        std::chrono::steady_clock::time_point last_{ std::chrono::steady_clock::now() };

        void Start() { last_ = std::chrono::steady_clock::now(); }
        void Stop()
        {
            sum_us_ += std::chrono::duration_cast< std::chrono::duration< double, std::micro > >( std::chrono::steady_clock::now() - last_ ).count();
            ++count_;
        }
    };

    template< typename T, std::size_t size >
    std::array< T, size > GenerateRandomArray( T const min_val, T const max_val )
    {
        std::array< T, size > a;

        std::mt19937 rng{ std::random_device{}() };
        if constexpr( std::is_floating_point_v< T > )
        {
            std::uniform_real_distribution< T > dist{ min_val, max_val };
            std::generate( a.begin(), a.end(), [ & ] { return dist( rng ); } );
        }
        else
        {
            std::uniform_int_distribution< T > dist{ min_val, max_val };
            std::generate( a.begin(), a.end(), [ & ] { return dist( rng ); } );
        }

        return a;
    }
}

#define micah_time( x ) micah::Timer micah##x( #x )
#define micah_stop( x ) micah##x.check_duration()
