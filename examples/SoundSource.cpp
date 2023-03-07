#include <dshello.hpp>
#include <DataSource.hpp>
#include <SoundSource.hpp>

template class DataSource<SAMPLE>;


SoundSource::SoundSource(uint1 n) 
: DataSource<SAMPLE>(n) 
{ }

SoundSource::SoundSource(SoundSource && oSrc) noexcept 
: DataSource<SAMPLE>(move(oSrc)) 
{ }


