#!/usr/bin/env python3

import argparse
import html
from collections import Counter
from datetime import UTC, datetime
from pathlib import Path
from urllib.parse import quote
import xml.etree.ElementTree as ET


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Render cppcheck XML as a self-contained HTML report.")
    parser.add_argument("--input", type=Path, default=Path("cppcheck.xml"))
    parser.add_argument("--output", type=Path, default=Path("cppcheck-report.html"))
    parser.add_argument("--repository", default="", help="GitHub owner/repository name.")
    parser.add_argument("--revision", default="", help="Git revision used for source links.")
    parser.add_argument("--run-url", default="", help="GitHub Actions run URL.")
    return parser.parse_args()


def source_url(repository: str, revision: str, file_name: str, line: str) -> str:
    if not repository or not revision or not file_name:
        return ""
    url = (
        f"https://github.com/{quote(repository, safe='/')}/blob/"
        f"{quote(revision, safe='')}/{quote(file_name, safe='/')}"
    )
    return f"{url}#L{line}" if line else url


def location_html(location: ET.Element, repository: str, revision: str) -> str:
    file_name = location.get("file", "")
    line = location.get("line", "")
    column = location.get("column", "")
    label = file_name or "Unknown location"
    if line:
        label += f":{line}"
        if column:
            label += f":{column}"

    escaped_label = html.escape(label)
    url = source_url(repository, revision, file_name, line)
    if url:
        return f'<a href="{html.escape(url, quote=True)}" target="_blank" rel="noopener">{escaped_label}</a>'
    return escaped_label


def render_finding(finding: ET.Element, repository: str, revision: str) -> str:
    finding_id = finding.get("id", "unknown")
    severity = finding.get("severity", "unknown")
    message = finding.get("msg", "")
    verbose = finding.get("verbose", "")
    cwe = finding.get("cwe", "")
    locations = finding.findall("location")

    primary_location = (
        location_html(locations[0], repository, revision)
        if locations
        else html.escape(finding.get("file0", "Unknown location"))
    )
    extra_locations = ""
    if len(locations) > 1:
        location_items = "".join(
            f"<li>{location_html(location, repository, revision)}</li>" for location in locations[1:]
        )
        extra_locations = f"<details><summary>Related locations</summary><ul>{location_items}</ul></details>"

    verbose_html = ""
    if verbose and verbose != message:
        verbose_html = f"<details><summary>Details</summary><p>{html.escape(verbose)}</p></details>"

    cwe_html = ""
    if cwe:
        cwe_url = f"https://cwe.mitre.org/data/definitions/{quote(cwe, safe='')}.html"
        cwe_html = f'<a href="{cwe_url}" target="_blank" rel="noopener">CWE-{html.escape(cwe)}</a>'

    return f"""
        <tr class="finding" data-severity="{html.escape(severity, quote=True)}">
          <td><span class="severity severity-{html.escape(severity, quote=True)}">{html.escape(severity)}</span></td>
          <td><code>{html.escape(finding_id)}</code></td>
          <td>{primary_location}{extra_locations}</td>
          <td>{html.escape(message)}{verbose_html}</td>
          <td>{cwe_html}</td>
        </tr>"""


def main() -> None:
    args = parse_args()
    # cppcheck writes XML to stderr.  When it reports no diagnostics, the
    # redirected output file exists but is empty, so there is no XML document
    # for ElementTree to parse.  Treat that as an empty report.
    xml_content = args.input.read_text(encoding="utf-8")
    root = ET.fromstring(xml_content) if xml_content.strip() else None
    findings = root.findall("./errors/error") if root is not None else []
    cppcheck = root.find("cppcheck") if root is not None else None
    cppcheck_version = cppcheck.get("version", "unknown") if cppcheck is not None else "unknown"
    counts = Counter(finding.get("severity", "unknown") for finding in findings)

    severity_order = ["error", "warning", "performance", "portability", "style", "information", "unknown"]
    severities = [severity for severity in severity_order if counts[severity]]
    severities.extend(sorted(set(counts) - set(severity_order)))

    cards = "".join(
        f'<div class="card"><span class="count">{counts[severity]}</span>'
        f'<span class="severity severity-{html.escape(severity, quote=True)}">{html.escape(severity)}</span></div>'
        for severity in severities
    )
    options = "".join(
        f'<option value="{html.escape(severity, quote=True)}">{html.escape(severity)} ({counts[severity]})</option>'
        for severity in severities
    )
    rows = "".join(render_finding(finding, args.repository, args.revision) for finding in findings)
    if not rows:
        rows = '<tr id="empty-report"><td colspan="5">No cppcheck findings were reported.</td></tr>'

    revision_label = args.revision[:12] if args.revision else "local"
    repository_label = args.repository or "local checkout"
    run_link = ""
    if args.run_url:
        run_link = (
            f' · <a href="{html.escape(args.run_url, quote=True)}" target="_blank" rel="noopener">'
            "workflow run</a>"
        )

    report = f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Cppcheck report · {html.escape(repository_label)}</title>
  <style>
    :root {{
      color-scheme: light dark;
      --background: #ffffff;
      --surface: #f6f8fa;
      --border: #d0d7de;
      --foreground: #1f2328;
      --muted: #59636e;
      --link: #0969da;
    }}
    @media (prefers-color-scheme: dark) {{
      :root {{
        --background: #0d1117;
        --surface: #161b22;
        --border: #30363d;
        --foreground: #f0f6fc;
        --muted: #9198a1;
        --link: #4493f8;
      }}
    }}
    * {{ box-sizing: border-box; }}
    body {{
      background: var(--background);
      color: var(--foreground);
      font: 14px/1.5 -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      margin: 0;
    }}
    main {{ margin: 0 auto; max-width: 1500px; padding: 32px 24px 64px; }}
    h1 {{ font-size: 28px; margin: 0 0 4px; }}
    a {{ color: var(--link); }}
    .metadata {{ color: var(--muted); margin: 0 0 24px; }}
    .cards {{ display: flex; flex-wrap: wrap; gap: 12px; margin-bottom: 24px; }}
    .card {{
      align-items: center;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 8px;
      display: flex;
      gap: 8px;
      padding: 10px 14px;
    }}
    .count {{ font-size: 20px; font-weight: 600; }}
    .controls {{ display: flex; gap: 12px; margin-bottom: 16px; }}
    input, select {{
      background: var(--background);
      border: 1px solid var(--border);
      border-radius: 6px;
      color: var(--foreground);
      font: inherit;
      padding: 8px 10px;
    }}
    input {{ flex: 1; min-width: 220px; }}
    .table-container {{ border: 1px solid var(--border); border-radius: 8px; overflow-x: auto; }}
    table {{ border-collapse: collapse; min-width: 1000px; width: 100%; }}
    th, td {{ border-bottom: 1px solid var(--border); padding: 10px 12px; text-align: left; vertical-align: top; }}
    th {{ background: var(--surface); position: sticky; top: 0; }}
    tr:last-child td {{ border-bottom: 0; }}
    code {{ font-family: ui-monospace, SFMono-Regular, Consolas, monospace; }}
    details {{ color: var(--muted); margin-top: 5px; }}
    details p, details ul {{ margin: 5px 0 0; }}
    .severity {{
      border: 1px solid currentColor;
      border-radius: 999px;
      display: inline-block;
      font-size: 12px;
      font-weight: 600;
      padding: 1px 7px;
    }}
    .severity-error {{ color: #cf222e; }}
    .severity-warning {{ color: #bf8700; }}
    .severity-performance {{ color: #8250df; }}
    .severity-portability {{ color: #0969da; }}
    .severity-style {{ color: #1a7f37; }}
    .severity-information {{ color: var(--muted); }}
    #no-results {{ color: var(--muted); display: none; padding: 24px; text-align: center; }}
  </style>
</head>
<body>
  <main>
    <h1>Cppcheck report</h1>
    <p class="metadata">
      {html.escape(repository_label)} @ {html.escape(revision_label)}
      · cppcheck {html.escape(cppcheck_version)}
      · generated {datetime.now(UTC).strftime("%Y-%m-%d %H:%M UTC")}
      {run_link}
    </p>
    <section class="cards">
      <div class="card"><span class="count">{len(findings)}</span><span>total findings</span></div>
      {cards}
    </section>
    <section class="controls" aria-label="Report filters">
      <input id="search" type="search" placeholder="Filter by file, check, or message…" aria-label="Filter findings">
      <select id="severity" aria-label="Filter by severity">
        <option value="">All severities ({len(findings)})</option>
        {options}
      </select>
    </section>
    <div class="table-container">
      <table>
        <thead>
          <tr><th>Severity</th><th>Check</th><th>Location</th><th>Message</th><th>CWE</th></tr>
        </thead>
        <tbody id="findings">{rows}</tbody>
      </table>
      <div id="no-results">No findings match the selected filters.</div>
    </div>
  </main>
  <script>
    const search = document.getElementById("search");
    const severity = document.getElementById("severity");
    const findings = Array.from(document.querySelectorAll("tr.finding"));
    const noResults = document.getElementById("no-results");

    function applyFilters() {{
      const query = search.value.trim().toLowerCase();
      const selectedSeverity = severity.value;
      let visible = 0;
      for (const finding of findings) {{
        const matchesText = !query || finding.textContent.toLowerCase().includes(query);
        const matchesSeverity = !selectedSeverity || finding.dataset.severity === selectedSeverity;
        const show = matchesText && matchesSeverity;
        finding.hidden = !show;
        if (show) visible++;
      }}
      noResults.style.display = findings.length && !visible ? "block" : "none";
    }}

    search.addEventListener("input", applyFilters);
    severity.addEventListener("change", applyFilters);
  </script>
</body>
</html>
"""
    args.output.write_text(report, encoding="utf-8")


if __name__ == "__main__":
    main()
