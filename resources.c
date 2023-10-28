#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.litos"), aligned (8)))
#else
# define SECTION
#endif

static const SECTION union { const guint8 data[5057]; const double alignment; void * const ptr;}  litos_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\344\000\000\000\000\000\000\050\007\000\000\000"
  "\000\000\000\000\001\000\000\000\003\000\000\000\004\000\000\000"
  "\005\000\000\000\006\000\000\000\007\000\000\000\106\120\345\042"
  "\006\000\000\000\344\000\000\000\010\000\166\000\360\000\000\000"
  "\135\007\000\000\113\120\220\013\003\000\000\000\135\007\000\000"
  "\004\000\114\000\144\007\000\000\150\007\000\000\100\364\277\000"
  "\001\000\000\000\150\007\000\000\004\000\114\000\154\007\000\000"
  "\160\007\000\000\324\265\002\000\377\377\377\377\160\007\000\000"
  "\001\000\114\000\164\007\000\000\170\007\000\000\232\166\370\170"
  "\006\000\000\000\170\007\000\000\015\000\166\000\210\007\000\000"
  "\112\014\000\000\076\077\246\020\006\000\000\000\112\014\000\000"
  "\011\000\166\000\130\014\000\000\256\023\000\000\072\330\020\030"
  "\002\000\000\000\256\023\000\000\006\000\114\000\264\023\000\000"
  "\300\023\000\000\160\162\145\146\163\056\165\151\000\000\000\000"
  "\135\006\000\000\000\000\000\000\074\077\170\155\154\040\166\145"
  "\162\163\151\157\156\075\042\061\056\060\042\040\145\156\143\157"
  "\144\151\156\147\075\042\125\124\106\055\070\042\077\076\012\074"
  "\151\156\164\145\162\146\141\143\145\076\012\040\040\074\164\145"
  "\155\160\154\141\164\145\040\143\154\141\163\163\075\042\114\151"
  "\164\157\163\101\160\160\120\162\145\146\163\042\040\160\141\162"
  "\145\156\164\075\042\107\164\153\104\151\141\154\157\147\042\076"
  "\012\040\040\040\040\074\160\162\157\160\145\162\164\171\040\156"
  "\141\155\145\075\042\164\151\164\154\145\042\040\164\162\141\156"
  "\163\154\141\164\141\142\154\145\075\042\171\145\163\042\076\120"
  "\162\145\146\145\162\145\156\143\145\163\074\057\160\162\157\160"
  "\145\162\164\171\076\012\040\040\040\040\074\160\162\157\160\145"
  "\162\164\171\040\156\141\155\145\075\042\162\145\163\151\172\141"
  "\142\154\145\042\076\060\074\057\160\162\157\160\145\162\164\171"
  "\076\012\040\040\040\040\074\160\162\157\160\145\162\164\171\040"
  "\156\141\155\145\075\042\155\157\144\141\154\042\076\061\074\057"
  "\160\162\157\160\145\162\164\171\076\012\040\040\040\040\074\143"
  "\150\151\154\144\040\151\156\164\145\162\156\141\154\055\143\150"
  "\151\154\144\075\042\143\157\156\164\145\156\164\137\141\162\145"
  "\141\042\076\012\040\040\040\040\040\040\074\157\142\152\145\143"
  "\164\040\143\154\141\163\163\075\042\107\164\153\102\157\170\042"
  "\040\151\144\075\042\143\157\156\164\145\156\164\137\141\162\145"
  "\141\042\076\012\040\040\040\040\040\040\040\040\074\143\150\151"
  "\154\144\076\012\040\040\040\040\040\040\040\040\040\040\074\157"
  "\142\152\145\143\164\040\143\154\141\163\163\075\042\107\164\153"
  "\107\162\151\144\042\040\151\144\075\042\147\162\151\144\042\076"
  "\012\040\040\040\040\040\040\040\040\040\040\040\040\074\160\162"
  "\157\160\145\162\164\171\040\156\141\155\145\075\042\155\141\162"
  "\147\151\156\055\163\164\141\162\164\042\076\061\062\074\057\160"
  "\162\157\160\145\162\164\171\076\012\040\040\040\040\040\040\040"
  "\040\040\040\040\040\074\160\162\157\160\145\162\164\171\040\156"
  "\141\155\145\075\042\155\141\162\147\151\156\055\145\156\144\042"
  "\076\061\062\074\057\160\162\157\160\145\162\164\171\076\012\040"
  "\040\040\040\040\040\040\040\040\040\040\040\074\160\162\157\160"
  "\145\162\164\171\040\156\141\155\145\075\042\155\141\162\147\151"
  "\156\055\164\157\160\042\076\061\062\074\057\160\162\157\160\145"
  "\162\164\171\076\012\040\040\040\040\040\040\040\040\040\040\040"
  "\040\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075"
  "\042\155\141\162\147\151\156\055\142\157\164\164\157\155\042\076"
  "\061\062\074\057\160\162\157\160\145\162\164\171\076\012\040\040"
  "\040\040\040\040\040\040\040\040\040\040\074\160\162\157\160\145"
  "\162\164\171\040\156\141\155\145\075\042\162\157\167\055\163\160"
  "\141\143\151\156\147\042\076\061\062\074\057\160\162\157\160\145"
  "\162\164\171\076\012\040\040\040\040\040\040\040\040\040\040\040"
  "\040\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075"
  "\042\143\157\154\165\155\156\055\163\160\141\143\151\156\147\042"
  "\076\061\062\074\057\160\162\157\160\145\162\164\171\076\012\040"
  "\040\040\040\040\040\040\040\040\040\040\040\074\143\150\151\154"
  "\144\076\012\040\040\040\040\040\040\040\040\040\040\040\040\040"
  "\040\074\157\142\152\145\143\164\040\143\154\141\163\163\075\042"
  "\107\164\153\114\141\142\145\154\042\040\151\144\075\042\146\157"
  "\156\164\154\141\142\145\154\042\076\012\040\040\040\040\040\040"
  "\040\040\040\040\040\040\040\040\040\040\074\160\162\157\160\145"
  "\162\164\171\040\156\141\155\145\075\042\154\141\142\145\154\042"
  "\076\137\106\157\156\164\072\074\057\160\162\157\160\145\162\164"
  "\171\076\012\040\040\040\040\040\040\040\040\040\040\040\040\040"
  "\040\040\040\074\160\162\157\160\145\162\164\171\040\156\141\155"
  "\145\075\042\165\163\145\055\165\156\144\145\162\154\151\156\145"
  "\042\076\061\074\057\160\162\157\160\145\162\164\171\076\012\040"
  "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\074"
  "\160\162\157\160\145\162\164\171\040\156\141\155\145\075\042\155"
  "\156\145\155\157\156\151\143\055\167\151\144\147\145\164\042\076"
  "\146\157\156\164\074\057\160\162\157\160\145\162\164\171\076\012"
  "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
  "\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075\042"
  "\170\141\154\151\147\156\042\076\061\074\057\160\162\157\160\145"
  "\162\164\171\076\012\040\040\040\040\040\040\040\040\040\040\040"
  "\040\040\040\040\040\074\154\141\171\157\165\164\076\012\040\040"
  "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
  "\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075\042"
  "\143\157\154\165\155\156\042\076\060\074\057\160\162\157\160\145"
  "\162\164\171\076\012\040\040\040\040\040\040\040\040\040\040\040"
  "\040\040\040\040\040\040\040\074\160\162\157\160\145\162\164\171"
  "\040\156\141\155\145\075\042\162\157\167\042\076\060\074\057\160"
  "\162\157\160\145\162\164\171\076\012\040\040\040\040\040\040\040"
  "\040\040\040\040\040\040\040\040\040\074\057\154\141\171\157\165"
  "\164\076\012\040\040\040\040\040\040\040\040\040\040\040\040\040"
  "\040\074\057\157\142\152\145\143\164\076\012\040\040\040\040\040"
  "\040\040\040\040\040\040\040\074\057\143\150\151\154\144\076\012"
  "\040\040\040\040\040\040\040\040\040\040\040\040\074\143\150\151"
  "\154\144\076\012\040\040\040\040\040\040\040\040\040\040\040\040"
  "\040\040\074\157\142\152\145\143\164\040\143\154\141\163\163\075"
  "\042\107\164\153\106\157\156\164\102\165\164\164\157\156\042\040"
  "\151\144\075\042\146\157\156\164\042\076\012\040\040\040\040\040"
  "\040\040\040\040\040\040\040\040\040\040\040\074\154\141\171\157"
  "\165\164\076\012\040\040\040\040\040\040\040\040\040\040\040\040"
  "\040\040\040\040\040\040\074\160\162\157\160\145\162\164\171\040"
  "\156\141\155\145\075\042\143\157\154\165\155\156\042\076\061\074"
  "\057\160\162\157\160\145\162\164\171\076\012\040\040\040\040\040"
  "\040\040\040\040\040\040\040\040\040\040\040\040\040\074\160\162"
  "\157\160\145\162\164\171\040\156\141\155\145\075\042\162\157\167"
  "\042\076\060\074\057\160\162\157\160\145\162\164\171\076\012\040"
  "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\074"
  "\057\154\141\171\157\165\164\076\012\040\040\040\040\040\040\040"
  "\040\040\040\040\040\040\040\074\057\157\142\152\145\143\164\076"
  "\012\040\040\040\040\040\040\040\040\040\040\040\040\074\057\143"
  "\150\151\154\144\076\012\040\040\040\040\040\040\040\040\040\040"
  "\040\040\074\057\157\142\152\145\143\164\076\012\040\040\040\040"
  "\040\040\040\040\074\057\143\150\151\154\144\076\012\040\040\040"
  "\040\040\040\074\057\157\142\152\145\143\164\076\012\040\040\040"
  "\040\074\057\143\150\151\154\144\076\012\040\040\074\057\164\145"
  "\155\160\154\141\164\145\076\012\074\057\151\156\164\145\162\146"
  "\141\143\145\076\012\000\000\050\165\165\141\171\051\157\162\147"
  "\057\000\000\000\002\000\000\000\147\164\153\057\006\000\000\000"
  "\057\000\000\000\001\000\000\000\147\145\141\162\163\055\155\145"
  "\156\165\056\165\151\000\000\000\262\004\000\000\000\000\000\000"
  "\074\077\170\155\154\040\166\145\162\163\151\157\156\075\042\061"
  "\056\060\042\040\145\156\143\157\144\151\156\147\075\042\125\124"
  "\106\055\070\042\077\076\012\074\151\156\164\145\162\146\141\143"
  "\145\076\012\040\040\074\155\145\156\165\040\151\144\075\042\155"
  "\145\156\165\042\076\012\040\040\040\040\074\163\145\143\164\151"
  "\157\156\076\012\040\040\040\040\040\040\074\151\164\145\155\076"
  "\012\040\040\040\040\040\040\040\040\074\141\164\164\162\151\142"
  "\165\164\145\040\156\141\155\145\075\042\154\141\142\145\154\042"
  "\040\164\162\141\156\163\154\141\164\141\142\154\145\075\042\171"
  "\145\163\042\076\137\120\162\145\146\145\162\145\156\143\145\163"
  "\074\057\141\164\164\162\151\142\165\164\145\076\012\040\040\040"
  "\040\040\040\040\040\074\141\164\164\162\151\142\165\164\145\040"
  "\156\141\155\145\075\042\141\143\164\151\157\156\042\076\141\160"
  "\160\056\160\162\145\146\145\162\145\156\143\145\163\074\057\141"
  "\164\164\162\151\142\165\164\145\076\012\040\040\040\040\040\040"
  "\074\057\151\164\145\155\076\012\040\040\040\040\074\057\163\145"
  "\143\164\151\157\156\076\012\040\040\040\040\074\163\145\143\164"
  "\151\157\156\076\012\040\040\040\040\040\040\074\151\164\145\155"
  "\076\012\040\040\040\040\040\040\040\040\074\141\164\164\162\151"
  "\142\165\164\145\040\156\141\155\145\075\042\154\141\142\145\154"
  "\042\040\164\162\141\156\163\154\141\164\141\142\154\145\075\042"
  "\171\145\163\042\076\137\116\145\167\074\057\141\164\164\162\151"
  "\142\165\164\145\076\012\040\040\040\040\040\040\040\040\074\141"
  "\164\164\162\151\142\165\164\145\040\156\141\155\145\075\042\141"
  "\143\164\151\157\156\042\076\141\160\160\056\156\145\167\074\057"
  "\141\164\164\162\151\142\165\164\145\076\012\040\040\040\040\040"
  "\040\074\057\151\164\145\155\076\012\040\040\040\040\074\057\163"
  "\145\143\164\151\157\156\076\012\040\040\040\040\074\163\145\143"
  "\164\151\157\156\076\012\040\040\040\040\040\040\074\151\164\145"
  "\155\076\012\040\040\040\040\040\040\040\040\074\141\164\164\162"
  "\151\142\165\164\145\040\156\141\155\145\075\042\154\141\142\145"
  "\154\042\040\164\162\141\156\163\154\141\164\141\142\154\145\075"
  "\042\171\145\163\042\076\137\117\160\145\156\074\057\141\164\164"
  "\162\151\142\165\164\145\076\012\040\040\040\040\040\040\040\040"
  "\074\141\164\164\162\151\142\165\164\145\040\156\141\155\145\075"
  "\042\141\143\164\151\157\156\042\076\141\160\160\056\157\160\145"
  "\156\074\057\141\164\164\162\151\142\165\164\145\076\012\040\040"
  "\040\040\040\040\074\057\151\164\145\155\076\012\040\040\040\040"
  "\074\057\163\145\143\164\151\157\156\076\012\040\040\040\040\074"
  "\163\145\143\164\151\157\156\076\012\040\040\040\040\040\040\074"
  "\151\164\145\155\076\012\040\040\040\040\040\040\040\040\074\141"
  "\164\164\162\151\142\165\164\145\040\156\141\155\145\075\042\154"
  "\141\142\145\154\042\040\164\162\141\156\163\154\141\164\141\142"
  "\154\145\075\042\171\145\163\042\076\116\145\167\040\106\162\157"
  "\155\040\124\145\155\160\154\141\164\145\074\057\141\164\164\162"
  "\151\142\165\164\145\076\012\040\040\040\040\040\040\040\040\074"
  "\141\164\164\162\151\142\165\164\145\040\156\141\155\145\075\042"
  "\141\143\164\151\157\156\042\076\141\160\160\056\157\160\145\156"
  "\137\164\155\160\154\074\057\141\164\164\162\151\142\165\164\145"
  "\076\012\040\040\040\040\040\040\074\057\151\164\145\155\076\012"
  "\040\040\040\040\074\057\163\145\143\164\151\157\156\076\012\040"
  "\040\040\040\074\163\145\143\164\151\157\156\076\012\040\040\040"
  "\040\040\040\074\151\164\145\155\076\012\040\040\040\040\040\040"
  "\040\040\074\141\164\164\162\151\142\165\164\145\040\156\141\155"
  "\145\075\042\154\141\142\145\154\042\040\164\162\141\156\163\154"
  "\141\164\141\142\154\145\075\042\171\145\163\042\076\137\123\141"
  "\166\145\137\101\163\074\057\141\164\164\162\151\142\165\164\145"
  "\076\012\040\040\040\040\040\040\040\040\074\141\164\164\162\151"
  "\142\165\164\145\040\156\141\155\145\075\042\141\143\164\151\157"
  "\156\042\076\141\160\160\056\163\141\166\145\137\141\163\074\057"
  "\141\164\164\162\151\142\165\164\145\076\012\040\040\040\040\040"
  "\040\074\057\151\164\145\155\076\012\040\040\040\040\074\057\163"
  "\145\143\164\151\157\156\076\012\040\040\040\040\074\163\145\143"
  "\164\151\157\156\076\012\040\040\040\040\040\040\074\151\164\145"
  "\155\076\012\040\040\040\040\040\040\040\040\074\141\164\164\162"
  "\151\142\165\164\145\040\156\141\155\145\075\042\154\141\142\145"
  "\154\042\040\164\162\141\156\163\154\141\164\141\142\154\145\075"
  "\042\171\145\163\042\076\137\121\165\151\164\074\057\141\164\164"
  "\162\151\142\165\164\145\076\012\040\040\040\040\040\040\040\040"
  "\074\141\164\164\162\151\142\165\164\145\040\156\141\155\145\075"
  "\042\141\143\164\151\157\156\042\076\141\160\160\056\161\165\151"
  "\164\074\057\141\164\164\162\151\142\165\164\145\076\012\040\040"
  "\040\040\040\040\074\057\151\164\145\155\076\012\040\040\040\040"
  "\074\057\163\145\143\164\151\157\156\076\012\040\040\074\057\155"
  "\145\156\165\076\012\074\057\151\156\164\145\162\146\141\143\145"
  "\076\012\000\000\050\165\165\141\171\051\167\151\156\144\157\167"
  "\056\165\151\000\000\000\000\000\106\007\000\000\000\000\000\000"
  "\074\077\170\155\154\040\166\145\162\163\151\157\156\075\042\061"
  "\056\060\042\040\145\156\143\157\144\151\156\147\075\042\125\124"
  "\106\055\070\042\077\076\012\074\151\156\164\145\162\146\141\143"
  "\145\076\012\040\040\074\164\145\155\160\154\141\164\145\040\143"
  "\154\141\163\163\075\042\114\151\164\157\163\101\160\160\127\151"
  "\156\144\157\167\042\040\160\141\162\145\156\164\075\042\107\164"
  "\153\101\160\160\154\151\143\141\164\151\157\156\127\151\156\144"
  "\157\167\042\076\012\040\040\040\040\074\160\162\157\160\145\162"
  "\164\171\040\156\141\155\145\075\042\164\151\164\154\145\042\076"
  "\114\151\164\157\163\074\057\160\162\157\160\145\162\164\171\076"
  "\012\040\040\040\040\074\143\150\151\154\144\040\164\171\160\145"
  "\075\042\164\151\164\154\145\142\141\162\042\076\012\040\040\040"
  "\040\040\040\074\157\142\152\145\143\164\040\143\154\141\163\163"
  "\075\042\107\164\153\110\145\141\144\145\162\102\141\162\042\040"
  "\151\144\075\042\150\145\141\144\145\162\042\076\012\040\040\040"
  "\040\040\040\040\040\074\143\150\151\154\144\040\164\171\160\145"
  "\075\042\145\156\144\042\076\012\040\040\040\040\040\040\040\040"
  "\040\040\074\157\142\152\145\143\164\040\143\154\141\163\163\075"
  "\042\107\164\153\124\157\147\147\154\145\102\165\164\164\157\156"
  "\042\040\151\144\075\042\163\145\141\162\143\150\042\076\012\040"
  "\040\040\040\040\040\040\040\040\040\040\040\074\160\162\157\160"
  "\145\162\164\171\040\156\141\155\145\075\042\163\145\156\163\151"
  "\164\151\166\145\042\076\060\074\057\160\162\157\160\145\162\164"
  "\171\076\012\040\040\040\040\040\040\040\040\040\040\040\040\074"
  "\160\162\157\160\145\162\164\171\040\156\141\155\145\075\042\151"
  "\143\157\156\055\156\141\155\145\042\076\145\144\151\164\055\146"
  "\151\156\144\055\163\171\155\142\157\154\151\143\074\057\160\162"
  "\157\160\145\162\164\171\076\012\040\040\040\040\040\040\040\040"
  "\040\040\074\057\157\142\152\145\143\164\076\012\040\040\040\040"
  "\040\040\040\040\074\057\143\150\151\154\144\076\012\011\074\143"
  "\150\151\154\144\040\164\171\160\145\075\042\145\156\144\042\076"
  "\012\040\040\040\040\040\040\040\040\040\040\074\157\142\152\145"
  "\143\164\040\143\154\141\163\163\075\042\107\164\153\102\165\164"
  "\164\157\156\042\040\151\144\075\042\141\142\157\165\164\042\076"
  "\012\040\040\040\040\040\040\040\040\040\040\040\040\074\160\162"
  "\157\160\145\162\164\171\040\156\141\155\145\075\042\151\143\157"
  "\156\055\156\141\155\145\042\076\154\157\147\157\055\151\143\157"
  "\156\055\141\142\157\165\164\074\057\160\162\157\160\145\162\164"
  "\171\076\012\040\040\040\040\040\040\040\040\040\040\040\040\074"
  "\163\151\147\156\141\154\040\156\141\155\145\075\042\143\154\151"
  "\143\153\145\144\042\040\150\141\156\144\154\145\162\075\042\141"
  "\142\157\165\164\137\144\151\141\154\157\147\042\057\076\012\040"
  "\040\040\040\040\040\040\040\040\040\074\057\157\142\152\145\143"
  "\164\076\012\040\040\040\040\040\040\040\040\074\057\143\150\151"
  "\154\144\076\012\040\040\040\040\040\040\040\040\074\143\150\151"
  "\154\144\040\164\171\160\145\075\042\163\164\141\162\164\042\076"
  "\012\040\040\040\040\040\040\040\040\040\040\074\157\142\152\145"
  "\143\164\040\143\154\141\163\163\075\042\107\164\153\115\145\156"
  "\165\102\165\164\164\157\156\042\040\151\144\075\042\147\145\141"
  "\162\163\042\076\012\040\040\040\040\040\040\040\040\040\040\040"
  "\040\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075"
  "\042\144\151\162\145\143\164\151\157\156\042\076\156\157\156\145"
  "\074\057\160\162\157\160\145\162\164\171\076\012\040\040\040\040"
  "\040\040\040\040\040\040\074\057\157\142\152\145\143\164\076\012"
  "\040\040\040\040\040\040\040\040\074\057\143\150\151\154\144\076"
  "\012\040\040\040\040\040\040\074\057\157\142\152\145\143\164\076"
  "\012\040\040\040\040\074\057\143\150\151\154\144\076\012\040\040"
  "\040\040\074\143\150\151\154\144\076\012\040\040\040\040\040\040"
  "\074\157\142\152\145\143\164\040\143\154\141\163\163\075\042\107"
  "\164\153\102\157\170\042\040\151\144\075\042\143\157\156\164\145"
  "\156\164\137\142\157\170\042\076\012\040\040\040\040\040\040\040"
  "\040\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075"
  "\042\157\162\151\145\156\164\141\164\151\157\156\042\076\166\145"
  "\162\164\151\143\141\154\074\057\160\162\157\160\145\162\164\171"
  "\076\012\040\040\040\040\040\040\040\040\074\143\150\151\154\144"
  "\076\012\040\040\040\040\040\040\040\040\040\040\074\157\142\152"
  "\145\143\164\040\143\154\141\163\163\075\042\107\164\153\123\145"
  "\141\162\143\150\102\141\162\042\040\151\144\075\042\163\145\141"
  "\162\143\150\142\141\162\042\076\012\011\040\040\074\143\150\151"
  "\154\144\076\012\040\040\040\040\040\040\040\040\040\040\040\040"
  "\074\157\142\152\145\143\164\040\143\154\141\163\163\075\042\107"
  "\164\153\102\157\170\042\040\151\144\075\042\163\145\141\162\143"
  "\150\137\142\157\170\042\076\012\040\040\040\040\040\040\040\040"
  "\040\040\040\040\074\143\150\151\154\144\076\012\040\040\040\040"
  "\040\040\040\040\040\040\040\040\040\040\074\157\142\152\145\143"
  "\164\040\143\154\141\163\163\075\042\107\164\153\123\145\141\162"
  "\143\150\105\156\164\162\171\042\040\151\144\075\042\163\145\141"
  "\162\143\150\145\156\164\162\171\042\076\012\040\040\040\040\040"
  "\040\040\040\040\040\040\040\040\040\040\040\074\163\151\147\156"
  "\141\154\040\156\141\155\145\075\042\163\145\141\162\143\150\055"
  "\143\150\141\156\147\145\144\042\040\150\141\156\144\154\145\162"
  "\075\042\163\145\141\162\143\150\137\164\145\170\164\137\143\150"
  "\141\156\147\145\144\042\057\076\012\040\040\040\040\040\040\040"
  "\040\040\040\040\040\040\040\074\057\157\142\152\145\143\164\076"
  "\012\040\040\040\040\040\040\040\040\040\040\040\040\074\057\143"
  "\150\151\154\144\076\012\011\040\040\040\040\074\143\150\151\154"
  "\144\076\012\011\040\040\040\040\040\040\074\157\142\152\145\143"
  "\164\040\143\154\141\163\163\075\042\107\164\153\102\165\164\164"
  "\157\156\042\040\151\144\075\042\165\160\137\163\145\141\162\143"
  "\150\042\076\012\011\040\040\040\040\040\040\040\040\074\160\162"
  "\157\160\145\162\164\171\040\156\141\155\145\075\042\151\143\157"
  "\156\055\156\141\155\145\042\076\147\157\055\165\160\074\057\160"
  "\162\157\160\145\162\164\171\076\012\011\040\040\040\040\040\040"
  "\074\057\157\142\152\145\143\164\076\012\011\040\040\040\040\074"
  "\057\143\150\151\154\144\076\012\011\040\040\040\040\074\143\150"
  "\151\154\144\076\012\011\040\040\040\040\040\040\074\157\142\152"
  "\145\143\164\040\143\154\141\163\163\075\042\107\164\153\102\165"
  "\164\164\157\156\042\040\151\144\075\042\144\157\167\156\137\163"
  "\145\141\162\143\150\042\076\012\011\040\040\040\040\040\040\040"
  "\040\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075"
  "\042\151\143\157\156\055\156\141\155\145\042\076\147\157\055\144"
  "\157\167\156\074\057\160\162\157\160\145\162\164\171\076\012\011"
  "\040\040\040\040\040\040\074\057\157\142\152\145\143\164\076\012"
  "\011\040\040\040\074\057\143\150\151\154\144\076\012\040\040\040"
  "\040\040\040\040\040\040\074\057\157\142\152\145\143\164\076\012"
  "\040\040\040\040\040\040\040\040\074\057\143\150\151\154\144\076"
  "\012\011\074\057\157\142\152\145\143\164\076\012\011\074\057\143"
  "\150\151\154\144\076\012\040\040\040\040\040\040\040\040\074\143"
  "\150\151\154\144\076\012\011\074\157\142\152\145\143\164\040\143"
  "\154\141\163\163\075\042\107\164\153\116\157\164\145\142\157\157"
  "\153\042\040\151\144\075\042\156\157\164\145\142\157\157\153\042"
  "\076\012\011\074\057\157\142\152\145\143\164\076\012\040\040\040"
  "\040\040\040\040\040\074\057\143\150\151\154\144\076\012\040\040"
  "\040\040\040\040\074\057\157\142\152\145\143\164\076\012\040\040"
  "\040\040\074\057\143\150\151\154\144\076\012\040\040\074\057\164"
  "\145\155\160\154\141\164\145\076\012\074\057\151\156\164\145\162"
  "\146\141\143\145\076\012\000\000\050\165\165\141\171\051\154\151"
  "\164\157\163\057\004\000\000\000\000\000\000\000\005\000\000\000"
  "" };

static GStaticResource static_resource = { litos_resource_data.data, sizeof (litos_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_MODULE_EXPORT
GResource *litos_get_resource (void);
GResource *litos_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_CONSTRUCTOR_H__
#define __G_CONSTRUCTOR_H__

/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
/* Visual studio 2008 and later has _Pragma */

/*
 * Only try to include gslist.h if not already included via glib.h,
 * so that items using gconstructor.h outside of GLib (such as
 * GResources) continue to build properly.
 */
#ifndef __G_LIB_H__
#include "gslist.h"
#endif

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined (_MSC_VER)

#define G_HAS_CONSTRUCTORS 1

/* Pre Visual studio 2008 must use #pragma section */
#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _wrapper(void) { _func(); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (*p)(void) = _func ## _wrapper;

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _constructor(void) { atexit (_func); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */
#endif /* __G_CONSTRUCTOR_H__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(litosresource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(litosresource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(litosresource_destructor)
#endif
G_DEFINE_DESTRUCTOR(litosresource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void litosresource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void litosresource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}