
class Container;

class SoundSource : public DataSource<SAMPLE>
{

   friend class Container;

   public:

   SoundSource(uint1 n);

   SoundSource(SoundSource && oSrc) noexcept;

};

