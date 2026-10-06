export const meta = {
  name: 'dqix-name-insides',
  description: 'Name every parameter, local, struct tag and field inside already-ported DQIX functions; agents write plans to their own file only',
  phases: [{ title: 'Name' }, { title: 'Audit' }],
}

const ENV = typeof process === 'undefined' ? {} : process.env ?? {}
const SP = args?.naming ?? ENV.DQIX_NAMING_DIR ?? 'naming'
const LABEL = args?.label ?? ENV.DQIX_LABEL_REPO ?? '../dqix-label'
const GAME = args?.repo ?? ENV.DQIX_REPO ?? '../dqix-decomp'
const PATHS = `In the brief, $DQIX_LABEL_REPO is ${LABEL} and $DQIX_REPO is ${GAME}.`
const DIR = args?.dir ?? 'inside_all'
const OUT = args?.out ?? 'inside_out'
const BATCHES = args?.batches ?? [0, 1, 2, 3]

const SUMMARY = {
  type: 'object',
  properties: {
    out_path: { type: 'string' },
    batch: { type: 'number' },
    files: { type: 'number' },
    renames: { type: 'number' },
    edits: { type: 'number' },
    unresolved: { type: 'string' },
  },
  required: ['out_path', 'batch', 'files', 'renames', 'edits', 'unresolved'],
}

const VERDICT_SUMMARY = {
  type: 'object',
  properties: {
    out_path: { type: 'string' },
    batch: { type: 'number' },
    files: { type: 'number' },
    rejected_renames: { type: 'number' },
    rejected_edits: { type: 'number' },
    rewritten_comments: { type: 'number' },
    worst: { type: 'string' },
  },
  required: ['out_path', 'batch', 'files', 'rejected_renames', 'rejected_edits', 'rewritten_comments', 'worst'],
}

const SHAPE =
  `[{"file": "src/...cpp", "function": "Name", "confidence": "certain|probable|partial", ` +
  `"notes": "one sentence", "comment": "// line\\n// line", ` +
  `"renames": [{"old": "obj", "new": "combatant", "kind": "param|local|struct|field|enum", ` +
  `"scope": "func | struct:Tag | line:unique substring — omit it only when the file spells "old" ` +
  `for one thing and one thing only", "evidence": "..."}], ` +
  `"edits": [{"old": "short slots[1];", "new": "short slots[10];", "evidence": "..."}]}]`

const results = await pipeline(
  BATCHES,
  (n) => {
    const nn = String(n).padStart(2, '0')
    return agent(
      `Read the brief at ${SP}/BRIEF_INSIDES.md in full, then produce a rename plan for every ` +
        `file listed in ${SP}/${DIR}/batch${nn}.json.\n\n` +
        `Read each source file on disk first, in full. Work through its junk_identifiers list ` +
        `item by item; every one you do not rename is a gap you must justify in notes. Field ` +
        `names come from reading the other functions that touch the same object off the ` +
        `labeling-pass branch, not from guessing at an offset. Never reorder a declaration and ` +
        `never change a type outside an edits entry.\n\n` +
        `Write your plan as JSON to ${SP}/${OUT}/plan${nn}.json — one array entry per input file, ` +
        `shaped exactly:\n${SHAPE}\n\n` +
        `That output file is the only thing you may write. Never edit anything under ` +
        `${LABEL}. Then return the summary.\n\n${PATHS}`,
      { label: `inside:batch${n}`, phase: 'Name', schema: SUMMARY },
    )
  },
  (res) => {
    const nn = String(res.batch).padStart(2, '0')
    return agent(
      `You are auditing proposed identifier renames for a Dragon Quest IX decompilation. The ` +
        `brief is at ${SP}/BRIEF_INSIDES.md — read it, especially the rules on what a field name ` +
        `must rest on and on what counts as a type change. The proposals are in ${res.out_path}.\n\n` +
        `Check every one against the source on disk under ${LABEL} and ` +
        `against the callers and callees, read with ` +
        `"git -C ${LABEL} show labeling-pass:<path>". Reject a field or ` +
        `parameter name that asserts a purpose the evidence does not establish — unknown<offset> ` +
        `is the correct name for a field nobody has read. Reject an array extent whose count is ` +
        `not cited. Reject a rename that reads worse than what it replaces. Reject a comment that ` +
        `states an inference as fact or that dropped a caveat the old one carried, and supply ` +
        `better_comment when you can fix it. An empty reject list is a fine verdict when the work ` +
        `is sound.\n\n` +
        `Check every unscoped rename for collisions: grep the file for the "old" spelling, and if ` +
        `it stands for more than one thing there — the function's parameter and a prototype's, a ` +
        `member of two structs — do not simply reject it. Repair it with fix_renames, one entry ` +
        `per meaning, each carrying a scope of "func", "struct:Tag" or "line:<unique substring>". ` +
        `fix_renames replaces every proposed rename with that "old" spelling.\n\n` +
        `Write your verdicts as JSON to ${SP}/${OUT}/verdict${nn}.json, one entry per file:\n` +
        `[{"file": "src/...cpp", "reject_renames": ["old", ...], "reject_edits": ["old line", ...], ` +
        `"fix_renames": [{"old": "obj", "new": "combatant", "scope": "func"}], ` +
        `"better_comment": "// ...", "reason": "..."}]\n` +
        `Use null for better_comment when the proposed comment stands. That output file is the ` +
        `only thing you may write. Never edit anything under ${LABEL}. ` +
        `Then return the summary.\n\n${PATHS}`,
      { label: `insideaudit:batch${res.batch}`, phase: 'Audit', schema: VERDICT_SUMMARY },
    )
  },
)

return { results }
