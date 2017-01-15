import pyotherside
import html2text
import markdown
import jinja2
import pynliner

MARKDOWN_CSS='body{font-family:Ubuntu,arial,sans-serif;font-size:14px;line-height:1.33;background-color:#fff;padding:5px}body>:first-child{margin-top:0!important}body>:last-child{margin-bottom:0!important}a{color:#4183C4}a.absent{color:#c00}a.anchor{display:block;padding-left:30px;margin-left:-30px;cursor:pointer;position:absolute;top:0;left:0;bottom:0}h1,h2,h3,h4,h5,h6{padding:0;font-weight:700;-webkit-font-smoothing:antialiased;cursor:text;position:relative}h1,h2{color:#000}h1:hover a.anchor,h2:hover a.anchor,h3:hover a.anchor,h4:hover a.anchor,h5:hover a.anchor,h6:hover a.anchor{text-decoration:none}h1 code,h1 tt,h2 code,h2 tt,h3 code,h3 tt,h4 code,h4 tt,h5 code,h5 tt,h6 code,h6 tt{font-size:inherit}h1{font-size:28px}h2{font-size:24px;border-bottom:1px solid #ccc}h3{font-size:18px}h4{font-size:16px}h5,h6{font-size:14px}h6{color:#777}blockquote,dl,li,ol,p,pre,table,ul{margin:15px 0}hr{border:0;color:#ccc;height:4px;padding:0}body>h1:first-child,body>h1:first-child+h2,body>h2:first-child,body>h3:first-child,body>h4:first-child,body>h5:first-child,body>h6:first-child{margin-top:0;padding-top:0}h1 p,h2 p,h3 p,h4 p,h5 p,h6 p,ol :first-child,ul :first-child{margin-top:0}a:first-child h1,a:first-child h2,a:first-child h3,a:first-child h4,a:first-child h5,a:first-child h6{margin-top:0;padding-top:0}li p.first{display:inline-block}li{margin:0}ol,ul{padding-left:30px}dl,dl dt,dl dt:first-child{padding:0}dl dt{font-size:14px;font-weight:700;font-style:italic;margin:15px 0 5px}blockquote>:first-child,dl dd>:first-child,dl dt>:first-child{margin-top:0}blockquote>:last-child,dl dd>:last-child,dl dt>:last-child{margin-bottom:0}dl dd{margin:0 0 15px;padding:0 15px}blockquote{border-left:4px solid #ddd;padding:0 15px;color:#777}table tr td,table tr th{border:1px solid #ccc;margin:0;padding:6px 13px}table{padding:0;border-collapse:collapse}table tr{border-top:1px solid #ccc;background-color:#fff;margin:0;padding:0}table tr th{font-weight:700}table tr td :first-child,table tr th :first-child{margin-top:0}table tr td :last-child,table tr th :last-child{margin-bottom:0}span.align-center,span.align-right{clear:both}span.align-center,span.align-center>span,span.align-right,span.align-right>span,span.float-left,span.float-right,span.float-right>span,span.frame,span.frame>span{display:block;overflow:hidden}img{max-width:100%}span.frame>span{border:1px solid #ddd;float:left;margin:13px 0 0;padding:7px;width:auto}span.frame span img{display:block;float:left}span.frame span span{clear:both;color:#333;display:block;padding:5px 0 0}span.align-center>span{margin:13px auto 0;text-align:center}span.align-center span img{margin:0 auto;text-align:center}span.align-right>span{margin:13px 0 0;text-align:right}span.align-right span img{margin:0;text-align:right}span.float-left{margin-right:13px;float:left}span.float-left span{margin:13px 0 0}span.float-right{margin-left:13px;float:right}span.float-right>span{margin:13px auto 0;text-align:right}code,pre,tt{background-color:#f8f8f8}code,tt{margin:0 2px;padding:0 5px;white-space:nowrap;border:1px solid #eaeaea;border-radius:3px}pre code{margin:0;padding:0;white-space:pre;background:0 0}pre{border:1px solid #ccc;font-size:13px;line-height:19px;overflow:auto;padding:6px 10px;border-radius:3px}pre code,pre tt{background-color:transparent;border:none}.sig p{opacity:0.6;}'
PYGMENTS_CSS=".codehilite .hll{background-color:#ffc}.codehilite .c{color:#998;font-style:italic}.codehilite .err{color:#a61717;background-color:#e3d2d2}.codehilite .k,.codehilite .o{color:#000;font-weight:700}.codehilite .cm{color:#998;font-style:italic}.codehilite .cp{color:#999;font-weight:700;font-style:italic}.codehilite .c1{color:#998;font-style:italic}.codehilite .cs{color:#999;font-weight:700;font-style:italic}.codehilite .gd{color:#000;background-color:#fdd}.codehilite .ge{color:#000;font-style:italic}.codehilite .gr{color:#a00}.codehilite .gh{color:#999}.codehilite .gi{color:#000;background-color:#dfd}.codehilite .go{color:#888}.codehilite .gp{color:#555}.codehilite .gs{font-weight:700}.codehilite .gu{color:#aaa}.codehilite .gt{color:#a00}.codehilite .kc,.codehilite .kd,.codehilite .kn,.codehilite .kp,.codehilite .kr{color:#000;font-weight:700}.codehilite .kt{color:#458;font-weight:700}.codehilite .m{color:#099}.codehilite .s{color:#d01040}.codehilite .na{color:teal}.codehilite .nb{color:#0086B3}.codehilite .nc{color:#458;font-weight:700}.codehilite .no{color:teal}.codehilite .nd{color:#3c5d5d;font-weight:700}.codehilite .ni{color:purple}.codehilite .ne,.codehilite .nf,.codehilite .nl{color:#900;font-weight:700}.codehilite .nn{color:#555}.codehilite .nt{color:navy}.codehilite .nv{color:teal}.codehilite .ow{color:#000;font-weight:700}.codehilite .w{color:#bbb}.codehilite .mf,.codehilite .mh,.codehilite .mi,.codehilite .mo{color:#099}.codehilite .s2,.codehilite .sb,.codehilite .sc,.codehilite .sd,.codehilite .se,.codehilite .sh,.codehilite .si,.codehilite .sx{color:#d01040}.codehilite .sr{color:#009926}.codehilite .s1{color:#d01040}.codehilite .ss{color:#990073}.codehilite .bp{color:#999}.codehilite .vc,.codehilite .vg,.codehilite .vi{color:teal}.codehilite .il{color:#099}"
TEMPLATE = """<!DOCTYPE html>
<html>
  <head>
  </head>
  <body>
      {{content}}
      <span class="sig">
        {{ signature }}
      </span>
    </div>
  </body>
</html>
"""

PLAIN_TEXT_TEMPLATE="""
{{ content }}
{{ signature }}
"""
def render_markdown(text):
    return markdown.markdown(text)

def render_markdown_with_opts(text, opts):
    _log("Creating extension list")
    exts = _get_extensions(opts)
    _log(exts)
    return markdown.markdown(text, extensions=exts)

def html_to_plaintext(html):
    h = html2text.HTML2Text()
    h.wrap_links = True
    h.pad_tables = True
    h.mark_code = True
    h.inline_links = False
    h.ignore_images = True
    h.single_line_break = True
    return h.handle(html)

def render_template(html, sig):
    return jinja2.Template(TEMPLATE).render(content=html, signature=sig)

def inline_with_css_string(html, css):
    p = pynliner.Pynliner()
    p.from_string(html).with_cssString(css)
    return p.run()

def inline_css(html):
    p = pynliner.Pynliner()
    p.from_string(html).with_cssString(MARKDOWN_CSS).with_cssString(PYGMENTS_CSS)
    return p.run()

def build_message_part_map(body, signature, opts):
    html_body = render_markdown_with_opts(body, opts)
    html_sig = render_markdown(signature)
    html_part = render_template(html_body, html_sig)

    ptext_body = html_to_plaintext(html_body)
    plain_sig = html2text.html2text(html_sig)
    plain_part = jinja2.Template(PLAIN_TEXT_TEMPLATE).render(content=ptext_body, signature=plain_sig)

    md_part = jinja2.Template(PLAIN_TEXT_TEMPLATE).render(content=body, signature=signature)

    pretty_html = inline_css(html_part)

    parts = {
        "plain": plain_part,
        "markdown": md_part,
        "html": pretty_html
    }
    return parts

def _get_extensions(opts):
    extensions = []
    if opts.extras:
        extensions.append('markdown.extensions.extra')
    else:
        if opts.abbreviations:
            extensions.append('markdown.extensions.abbr')

        if opts.attributeList:
            extensions.append('markdown.extensions.attr_list')

        if opts.definitionLists:
            extensions.append('markdown.extensions.def_list')

        if opts.fencedCodeBlocks:
            extensions.append('markdown.extensions.fenced_code')

        if opts.footnotes:
            extensions.append('markdown.extensions.footnotes')

        if opts.tables:
            extensions.append('markdown.extensions.tables')

        if opts.smartStrong:
            extensions.append('markdown.extensions.smart_strong')

    if opts.admonition:
        extensions.append('markdown.extensions.admonition')

    if opts.headerId:
        extensions.append('markdown.extensions.headerid')

    if opts.metadata:
        extensions.append('markdown.extensions.meta')

    if opts.nl2br:
        extensions.append('markdown.extensions.nl2br')

    if opts.saneLists:
        extensions.append('markdown.extensions.sane_lists')

    if opts.smartyPants:
        extensions.append('markdown.extensions.smarty')

    if opts.toc:
        extensions.append('markdown.extensions.toc')

    if opts.wikilinks:
        extensions.append('markdown.extensions.wikilinks')

    if opts.codehilite:
        extensions.append('markdown.extensions.codehilite')

    return extensions

def _log(msg):
    pyotherside.send('log', msg)

