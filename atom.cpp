//
// Created by mkh on 19.10.2020.
//

#include "atom.h"
#include "filetypebox.hh"
#include "freebox.hh"
#include "movieheaderbox.hh"
#include "trackheaderbox.hh"
#include "mediaheaderbox.hh"
#include "handlerreferencebox.hh"
#include "videomediaheaderbox.hh"
#include "soundmediaheaderbox.hh"
#include "datareferencebox.hh"
#include "timetosamplebox.hh"
#include "compositionoffsetbox.hh"
#include "sampledescriptionbox.hh"
#include "avccbox.hh"
#include "hvccbox.hh"
#include "samplesizebox.hh"
#include "sampletochunkbox.hh"
#include "movieextendsheaderbox.hh"
#include "trackextendsbox.hh"
#include "moviefragmentheaderbox.hh"
#include "trackfragmentheaderbox.hh"
#include "elementarystreamdescriptorbox.h"
#include "trackfragmentrunbox.hh"
#include "mediadatabox.hh"

std::ostream & operator <<( std::ostream& out, const Atom& atom ) {
    atom.fout( out );
    return out;
}

Atom * Atom::make( std::istream & is, const TrunMap & trunMap ) {
    static std::string handler_type;

    std::unique_ptr< Atom > atom( new Atom( is ) );
    if( !atom->container() ) {
        is.seekg( atom->position() );

        if( *atom == Atom::ftyp ) {
            return new FileTypeBox( is, atom->size() );
        }
        else if( *atom == Atom::free ) {
            return new Free( is, atom->size() );
        }
        else if( *atom == Atom::mvhd ) {
            return new MovieHeaderBox( is, atom->size() );
        }
        else if( *atom == Atom::tkhd ) {
            return new TrackHeaderBox( is, atom->size() );
        }
        else if( *atom == Atom::mdhd ) {
            return new MediaHeaderBox( is, atom->size() );
        }
        else if( *atom == Atom::hdlr ) {
            return new HandlerReferenceBox( is, &handler_type );
        }
        else if( *atom == Atom::vmhd ) {
            return new VideoMediaHeaderBox( is, atom->size() );
        }
        else if( *atom == Atom::smhd ) {
            return new SoundMediaHeaderBox( is );
        }
        else if( *atom == Atom::dref ) {
            return new DataReferenceBox( is );
        }
        else if( *atom == Atom::url ) {
            return new DataEntryUrlBox( is );
        }
        else if( *atom == Atom::urn ) {
            return new DataEntryUrnBox( is );
        }
        else if( *atom == Atom::stts ) {
            return new TimeToSampleBox( is );
        }
        else if( *atom == Atom::ctts ) {
            return new CompositionOffsetBox( is );
        }
        else if( *atom == Atom::stsd ) {
            return new SampleDescriptionBox( is, handler_type );
        }
        else if( *atom == Atom::avcc ) {
            return new AvcCBox( is );
        }
        else if( *atom == Atom::hvcc ) {
            return new HvcCBox( is );
        }
        else if( *atom == Atom::esds ) {
            return new ElementaryStreamDescriptorBox( is );
        }
        else if( *atom == Atom::stsz ) {
            return new SampleSizeBox( is );
        }
        else if( *atom == Atom::stsc ) {
            return new SampleToChunkBox( is );
        }
        else if( *atom == Atom::mehd ) {
            return new MovieExtendsHeaderBox( is );
        }
        else if( *atom == Atom::trex ) {
            return new TrackExtendsBox( is );
        }
        else if( *atom == Atom::mfhd ) {
            return new MovieFragmentHeaderBox( is );
        }
        else if( *atom == Atom::tfhd ) {
            return new TrackFragmentHeaderBox( is );
        }
        else if( *atom == Atom::trun ) {
            return new TrackFragmentRunBox( is );
        }
        else if( *atom == Atom::mdat ) {
            return new MediaDataBox( is, trunMap );
        }
        else
            is.seekg( atom->position() + std::streampos(atom->size()) );
    }

    return atom.release();
}

Atom::Atom( Value val ) {
    m_type.a = val;
}

Atom::Atom( std::istream& is ) {
    m_position = is.tellg();

    is.read( (char*)&m_size, sizeof(m_size) );
    m_size = be32toh(m_size);

    is.read( m_type.buf, sizeof(m_type.buf) );
    m_strtype = std::string( m_type.buf, sizeof(m_type.buf) );
}

Atom::operator uint32_t() const {
    return m_type.a;
}

const char * Atom::str() const {
    return m_strtype.c_str();
}

uint32_t Atom::size() const {
    return m_size;
}

std::streampos Atom::position() const {
    return m_position;
}

bool Atom::container() const {
    return m_type.a == Atom::moov ||
           m_type.a == Atom::edts ||
           m_type.a == Atom::trak ||
           m_type.a == Atom::mdia ||
           m_type.a == Atom::minf ||
           m_type.a == Atom::dinf ||
           m_type.a == Atom::stbl ||
           m_type.a == Atom::mvex ||
           m_type.a == Atom::moof ||
           m_type.a == Atom::traf ||
           m_type.a == Atom::udta;
}

void Atom::fout( std::ostream &out ) const {
    out << m_strtype << "(" << std::hex << htobe32(m_type.a) << ") " << std::dec << "size=" << m_size << " position=" << m_position;
}

void Atom::ctime( std::ostream & out, time_t t ) const {
    struct tm * tp = localtime( &t );
    tp->tm_year += 1904;

    out << tp->tm_mday << "/" << tp->tm_mon+1 << "/" << tp->tm_year << " "
        << tp->tm_hour << ":" << tp->tm_min << ":" << tp->tm_sec;
}