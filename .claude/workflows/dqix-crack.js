export const meta = {
  name: 'dqix-crack',
  description: 'Attack one DQIX residue with independent levers in bounded rounds, stopping on MATCH',
  whenToUse: 'A single function or idiom class sitting at a known BYTEDIFF with several plausible levers.',
  phases: [{ title: 'Round 1' }, { title: 'Round 2' }, { title: 'Round 3' }],
}

// args: { mod, addr, base, board, baseDiff, roundSize, levers: [{key, prompt}], sp, repo }
const A = args || {}
const MOD = A.mod
const ADDR = A.addr
const BASE = A.base
const BOARD = A.board
const BASEDIFF = A.baseDiff
const ROUND = A.roundSize || 3
const LEVERS = A.levers || []

const PATHS_SCHEMA = {
  type: 'object',
  properties: { kit: { type: 'string' }, sp: { type: 'string' }, repo: { type: 'string' } },
  required: ['kit', 'sp', 'repo'],
}
async function kitPaths() {
  if (A.kit && A.sp && A.repo) return { kit: A.kit, sp: A.sp, repo: A.repo }
  const root = A.kit || '${DQIX_KIT:-.}'
  const r = await agent(
    `Run exactly this one command and nothing else:
cd "${root}" && python -c "import kitpaths as k; print(k.KIT); print(k.SP); print(k.REPO)"
Return the first line it prints as kit, the second as sp and the third as repo, verbatim. Do not edit anything.`,
    { label: 'paths', schema: PATHS_SCHEMA, effort: 'low' })
  if (!r) throw new Error('could not resolve the kit paths; pass args.kit, args.sp and args.repo')
  return { kit: A.kit || r.kit, sp: A.sp || r.sp, repo: A.repo || r.repo }
}
const PATHS = await kitPaths()
const KIT = PATHS.kit
const SP = PATHS.sp
const REPO = PATHS.repo

const COMMON = `
TARGET: ${MOD} ${ADDR}. START FROM: ${BASE} (copy it, never edit it in place).
It currently gates a residue of ${BASEDIFF} bytes. ${BASEDIFF} is the number to beat; MATCH is the goal.

THE SHARED BOARD:
    ${BOARD}
It carries what is already solved and every lever already RULED OUT with its byte count. Repeating
anything on that list wastes money. Other agents are working this same residue right now.

**NEVER read a file larger than ~10KB.** Check first with \`wc -c\`. Reading a large file such as
\`OPEN_WORK.md\` once costs most of your context and most of your speed — a 400KB context takes MINUTES
per turn. If a note points you at a big file, read the small digest it names instead, or
\`grep\`/\`sed -n\` the few lines you need out of it. This is the single biggest thing that slows an
agent to a crawl on this project.

**READ THE BOARD IN FULL EXACTLY ONCE, at the start. After that read ONLY the last 25 lines**
(\`tail -25 ${BOARD}\`) to pick up what other agents have posted since. The board grows past 20KB and
re-reading it whole before every compile is what makes an agent slow to a crawl: your context grows
by the board plus every tool result, and a 400KB context takes MINUTES per turn. Guard your context
like a budget — it is the difference between 40 experiments and 8.

Same rule for every other command: pipe long output through \`grep\`/\`head\` so you read the few
lines you need, never a full disassembly or a compiler help dump. If you catch yourself pasting
large output you already have, stop and narrow the command.

APPEND ONE LINE AFTER EVERY GATED COMPILE, win or loss:
    echo "RESULT <yourlever> <bytediff> <the exact edit in a few words>" >> ${BOARD}

STOP CONDITIONS, checked before each compile by re-reading the board:
  - the board contains a MATCH  -> stop immediately and report what you had.
  - the board shows a result better than yours -> switch to THAT file and build on it, and say so.

RANK BY THE MAPPING, NOT BY THE BYTE COUNT. A lower BYTEDIFF can be the wrong basin: a residue
whose register mapping is a CONSISTENT permutation (a clean swap or 3-cycle, every instruction
agreeing) is one edit from MATCH, while a smaller residue whose mapping sends one register to two
different places is not colourable at all. Read the mapping wgate prints and prefer the consistent
one even when it is a few bytes worse. Say which you chose and why.

COMMANDS:
    gate:    cd ${REPO} && python ${KIT}/wgate.py ${MOD} ${ADDR} <yourfile>
    inspect: cd ${REPO} && python ${KIT}/wdiff.py ${MOD} ${ADDR} <yourfile>
    listing: python ${KIT}/wlist.py ${MOD} ${ADDR}
Concurrent gating is safe -- wgate uses a per-pid object file.

RULES: work only on your own copy under ${SP}/handwork/. Never edit src/, attempts/, the base file,
or another agent's file. Never write assembly. Never commit. Report the exact source edit, not your
reasoning. A negative result is valuable -- post it to the board and report it.
`

const SCHEMA = {
  type: 'object',
  properties: {
    lever: { type: 'string' },
    file: { type: 'string', description: 'absolute path of your best file' },
    best_verdict: { type: 'string', description: 'verbatim first line wgate printed for your best file' },
    best_bytediff: { type: 'integer', description: 'best reached; 0 for MATCH, 9999 if nothing compiled' },
    matched: { type: 'boolean', description: 'true only if wgate printed MATCH' },
    what_changed: { type: 'string', description: 'the exact source edit, as changed lines' },
    ruled_out: { type: 'string', description: 'what this lever definitively does not do' },
  },
  required: ['lever', 'file', 'best_verdict', 'best_bytediff', 'matched', 'what_changed', 'ruled_out'],
}

// ROUNDS, because parallel() is a barrier and the runtime cannot cancel a running agent. A single
// wave of N spends all N even after one of them matches at minute two -- measured on ov017:021941fc,
// where three agents kept working for several minutes past the MATCH. Rounds bound that waste to one
// round instead of the whole fan-out, and the board still gives cross-talk WITHIN a round.
const all = []
let winner = null
for (let i = 0; i < LEVERS.length && !winner; i += ROUND) {
  const slice = LEVERS.slice(i, i + ROUND)
  const title = `Round ${Math.floor(i / ROUND) + 1}`
  phase(title)
  log(`${title}: ${slice.map(l => l.key).join(', ')}`)
  const got = (await parallel(slice.map(l => () =>
    agent(COMMON + '\n' + l.prompt, { label: `lever:${l.key}`, phase: title, schema: SCHEMA }))))
    .filter(Boolean)
  all.push(...got)
  winner = got.find(r => r.matched) || null
  if (winner) log(`MATCH from ${winner.lever} -- no further rounds launched`)
}

const remaining = LEVERS.length - all.length
if (remaining > 0) log(`${remaining} lever(s) never launched (a MATCH ended the run)`)
const ranked = all.slice().sort((a, b) => a.best_bytediff - b.best_bytediff)
return { base: BASEDIFF, winner, best: ranked[0], all, levers_not_run: remaining }
