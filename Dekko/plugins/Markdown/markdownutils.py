import pyotherside
import markdown

def render_markdown(text, ext):
    return markdown.markdown(text, extensions=ext)
