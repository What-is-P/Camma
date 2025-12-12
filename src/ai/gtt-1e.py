# this code is based off gemini, a AI model by google, i think you were expecting claude or chatgpt, but we got gemini instead because that was the most accessible i could find.

import base64
import os
from google import genai
from google.genai import types


def generate():
    client = genai.Client(
        api_key=os.environ.get("GEMINI_API_KEY"),
    )

    model = "gemini-3-pro-preview"
    contents = [
        types.Content(
            role="user",
            parts=[
                types.Part.from_text(text="""INSERT_INPUT_HERE"""),
            ],
        ),
    ]
    tools = [
        types.Tool(url_context=types.UrlContext()),
        types.Tool(code_execution=types.ToolCodeExecution),
        types.Tool(googleSearch=types.GoogleSearch(
        )),
    ]
    generate_content_config = types.GenerateContentConfig(
        temperature=0.22,
        top_p=0.85,
        max_output_tokens=5200,
        thinkingConfig: {
            thinkingLevel: "HIGH",
        },
        media_resolution="MEDIA_RESOLUTION_MEDIUM",
        tools=tools,
    )

    for chunk in client.models.generate_content_stream(
        model=model,
        contents=contents,
        config=generate_content_config,
    ):
        if (
            chunk.candidates is None
            or chunk.candidates[0].content is None
            or chunk.candidates[0].content.parts is None
        ):
            continue
        if chunk.candidates[0].content.parts[0].text:
            print(chunk.candidates[0].content.parts[0].text, end="")
        if chunk.candidates[0].content.parts[0].executable_code:
            print(chunk.candidates[0].content.parts[0].executable_code)
        if chunk.candidates[0].content.parts[0].code_execution_result:
            print(chunk.candidates[0].content.parts[0].code_execution_result)

if __name__ == "__main__":
    generate()