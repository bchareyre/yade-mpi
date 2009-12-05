# source: http://www.scons.org/wiki/ReplacementBuilder
# used internally by scons, do not delete

from string import Template

def replace_action(target, source, env):
    open(str(target[0]),'w').write(Template(open(str(source[0]),'r').read()).safe_substitute(env))
    return 0

def replace_string(target, source, env):
    return None #"T %s %s" % (str(source[0]), str(target[0]))

def generate(env, **kw):
    action = env.Action(replace_action, replace_string)
    env['BUILDERS']['ScanReplace'] = env.Builder(action=action, src_suffix='.in', single_source=True)

def exists(env):
    return 1

